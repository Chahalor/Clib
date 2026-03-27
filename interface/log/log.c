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
	const t_log_init *const _data
)
{
	// va_list	_dummy;

	// return (_log_manager(e_log_mng_init, _dummy, _data));
	(void)_data;
	return (0);
}

/** */
int	logs_raw(
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
		.time = 0,// TODO
		.summary = (char *)summary,
		.body = (char *)body,
		.code = -1
	};

	return (0);
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
		.time = 0,// TODO
		.summary = (char *)summary,
		.body = (char *)body,
		.code = errno
	};

	return (0);
}

/** */
int	logs_report(
	const t_log_report *const report
)
{
	(void)report;

	return (0);
}

/** */
int	logs_perror_report(
	const t_log_report *const report
)
{
	(void)report;

	return (0);
}