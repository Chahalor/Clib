/**
 * @file log.c
 * @brief Public entry points for the `interface/log` module.
 *
 * This file contains wrappers that adapt public API calls to the internal
 * `_logs(...)` pipeline.
 */

/* ----| Headers    |----- */
	/* Standard */
#include <stdarg.h>
#include <errno.h>

	/* Internal */
		//...

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
#include "_internal_/_log.h"
#include "log.h"

/* ----| Public     |----- */

/**
 * @brief Initialize logging using the provided configuration.
 *
 * @param data Log initialization payload.
 *
 * @return `error_invalid_arg` when `data == NULL`.
 * @return `1` when `data != NULL` (current implementation behavior).
 *
 * @version 2.0.0
 *
 * @code{.c}
 * t_log_init init = {.display_limit = log_other};
 * int rc = log_init(&init);
 * @endcode
 */
int	log_init(
	const t_log_init *const data
)
{
	if (unlikely(!data))
		return (error_invalid_arg);

	return (_log_config(data) != NULL);
}

/**
 * @brief Raw-value logging implementation.
 *
 * Builds a stack `t_log_report` and forwards to `_logs(..., perr=false)`.
 * Variadic arguments are currently ignored.
 *
 * @return Forwarded result from `_logs(...)` (currently `error_none` on
 * successful write path).
 *
 * @version 2.0.0
 *
 * @code{.c}
 * logs_raw(log_info, __func__, __FILE__, __LINE__,
 *          0, "request complete", "latency=12ms");
 * @endcode
 */
int	logs_raw(
	const t_log_level level,
	const char *const func,
	const char *const file,
	const int line,
	const int code,
	const char *const summary,
	const char *const body, ...
)
{
	t_log_report	_report = {
		.level = level,
		.func = func,
		.file = file,
		.line = line,
		.summary = (char *)summary,
		.body = (char *)body,
		.code = code
	};

	return (_logs(&_report, true, false));
}

/**
 * @brief Raw-value logging implementation with `errno` capture.
 *
 * Sets `report.code = errno` and forwards to `_logs(..., perr=true)`.
 * Variadic arguments are currently ignored.
 *
 * @return Forwarded result from `_logs(...)` (currently `error_none` on
 * successful write path).
 *
 * @version 2.0.0
 *
 * @code{.c}
 * logs_perror_raw(log_error, __func__, __FILE__, __LINE__,
 *                 "open failed", "/tmp/app.conf");
 * @endcode
 */
int	logs_perror_raw(
	const t_log_level level,
	const char *const func,
	const char *const file,
	const int line,
	const char *const summary,
	const char *const body, ...
)
{
	t_log_report	_report = {
		.level = level,
		.func = func,
		.file = file,
		.line = line,
		.summary = (char *)summary,
		.body = (char *)body,
		.code = errno
	};

	return (_logs(&_report, true, true));
}

/**
 * @brief Report-based logging implementation.
 *
 * @param report Log report tree root.
 *
 * @return Forwarded result from `_logs(report, true, false)`.
 *
 * @version 2.0.0
 *
 * @code{.c}
 * t_log_report rep = {.level = log_debug, .summary = "state snapshot"};
 * logs_report(&rep);
 * @endcode
 */
int	logs_report(
	t_log_report *const report
)
{
	return (_logs(report, true, false));
}

/**
 * @brief Report-based logging implementation with `errno` detail.
 *
 * @param report Log report tree root.
 *
 * @return Forwarded result from `_logs(report, true, true)`.
 *
 * @version 2.0.0
 *
 * @code{.c}
 * t_log_report rep = {.level = log_error, .summary = "accept failed"};
 * logs_perror_report(&rep);
 * @endcode
 */
int	logs_perror_report(
	t_log_report *const report
)
{
	return (_logs(report, true, true));
}
