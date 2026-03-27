// Header

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
 * @brief		init the log manager based on the config
 * 
 * @param data	the config struct for the logs
 * 
 * @return error_none in case of success or the error code
 * 
 * @version 0.1
 */
int	log_init(
	const t_log_init *const data
)
{
	if (unlikely(!data))
		return (error_invalid_arg);

	return (_log_config(data) != NULL);
}

/** */
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

/** */
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

/** */
int	logs_report(
	t_log_report *const report
)
{
	return (_logs(report, true, false));
}

/** */
int	logs_perror_report(
	t_log_report *const report
)
{
	return (_logs(report, true, true));
}
