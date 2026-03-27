// Header

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

int	_logs(
	const t_log_report *const report,
	const bool atomic_safe
)
{
	int	_fd_file = _log_config(NULL)->fd[report->level];
	int	_fd_tty = _log_config(NULL)->display_limit > report->level ? -1 : STDERR_FILENO;
	int	result = error_none;

	if (_fd_tty > 0 && report->level == log_info)
		_fd_tty = STDOUT_FILENO;

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

	return (result);
}
