/**
 * @file _log.c
 * @brief Internal dispatcher implementation for `interface/log`.
 */

/* ----| Headers    |----- */
	/* Standard */
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

	/* Internal */
#include "_log.h"

	/* External */
#include "formating.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

/**
 * @brief Access/update internal logging singleton.
 *
 * @param init Optional initialization payload.
 *
 * @return Pointer to static singleton configuration.
 *
 * @code{.c}
 * t_log_internal *cfg = _log_config(NULL);
 * @endcode
 */
t_log_internal	*_log_config(
	const t_log_init *const init
)
{
	static t_log_internal	internal = {
		.fd = {-1}
	};

	if (init)
		_log_init(&internal, init);

	return (&internal);
}

/**
 * @brief Route one report to configured file and terminal outputs.
 *
 * Current behavior:
 * - file fd from `_log_config(NULL)->fd[level]`
 * - terminal fd enabled when `display_limit <= level`
 * - `log_info` uses `STDOUT_FILENO` when terminal output is enabled
 * - when `perr` is true, summary becomes `"summary: strerror(errno)"`
 *
 * @param report Mutable report payload.
 * @param atomic_safe Non-zero to use `_logs_print_atomic()`.
 * @param perr Non-zero to append `strerror(errno)` into summary.
 *
 * @return Last print function result (currently `0` on normal path).
 *
 * @code{.c}
 * t_log_report rep = {.level = log_warning, .summary = "slow query"};
 * int rc = _logs(&rep, true, false);
 * @endcode
 */
int	_logs(
	t_log_report *const report,
	const bool atomic_safe,
	const bool perr
)
{
	int		_fd_file = _log_config(NULL)->fd[report->level];
	int		_fd_tty = _log_config(NULL)->display_limit > report->level ? -1 : STDERR_FILENO;
	int		result = error_none;
	char	*backup = NULL;

	if (_fd_tty > 0 && report->level == log_info)
		_fd_tty = STDOUT_FILENO;

	if (perr)
	{
		const int		_errno = errno;
		const char		*_strerror = strerror(_errno);
		const size_t	_length = strlen(report->summary) + strlen(_strerror) + 1;
		char			*_new;

		_new = mem_alloc(sizeof(char) * _length);
		snprintf(_new, _length, "%s: %s", report->summary, _strerror);

		backup = report->summary;
		report->summary =_new;
	}

	if (atomic_safe)
	{
		if (_fd_file > 0)
			result = _logs_print_atomic(report, _fd_file);

		if (_fd_file != _fd_tty)
			result = _logs_print_atomic(report, _fd_tty);
	}
	else
	{
		if (_fd_file > 0)
			result = _logs_print(report, _fd_file, 0);

		if (_fd_file != _fd_tty)
			result = _logs_print(report, _fd_tty, 0);
	}

	if (backup)
	{
		mem_free(report->summary);
		report->summary = backup;
	}

	return (result);
}
