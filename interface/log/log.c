// Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdarg.h>
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
	t_log_init *_data
)
{
	va_list	_dummy;

	return (_log_manager(e_log_mng_init, _dummy, _data));
}

/**
 * @brief write to appropriate log file the log message
 * 
 * @param level		warning, debug, error, ...
 * @param depth		nb of tab in the log (if depth == -1 automaticly add one more tab than the previous log)
 * @param format	the format for printf
 * 
 * @return error_none in case of success or the error code
 * 
 * @version 0.1
 */
int	logs(
	int _level,
	int _depth,
	const char *_format,
	...
)
{
	int		result;
	va_list	_args;

	va_start(_args, _format);

	result = _log_manager(e_log_mng_write, _args, _level, _depth, _format);

	va_end(_args);
	return (result);
}

int	logs_perror(	// TODO: should add the sterror behind the message
	int level,
	int depth,
	const char *format,
	...
)
{
	va_list	_list;
	int		result = error_none;

	va_start(_list, format);
	result = _log_manager(e_log_mng_perror, _list, level, depth, format);
	va_end(_list);
	return (result);
}

int	logs_error(	// TODO: should add the errnum of lib str behind the message
	int level,
	int depth,
	const char *format,
	...
)
{
	(void)level;
	(void)depth;
	(void)format;
	return (error_not_implemented);
}

/**
 * @brief close one or all logs file
 * 
 * @param access	0, close only the level log file. 1, close all log file
 * @param level		the log level to close
 * 
 * @return error_none in case of success or the error code
 * 
 * @version 0.1
 */
int	log_close(
	const int access,
	const int level
)
{
	va_list	_dummy;

	if (access)
		return (_log_manager(e_log_mng_close, _dummy, (void *)&level, NULL));
	else
		return (_log_manager(e_log_mng_close_all, _dummy, NULL, NULL));
}
