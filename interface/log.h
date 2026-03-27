// Header

#ifndef LIB_INTERFACE_LOG_H
# define LIB_INTERFACE_LOG_H

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "../standards/lib_config.h"
# include "../standards/lib_standards.h"

/* -----| Internals |----- */
# include "log/types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

#define LOG_BUFFER_SIZE 8192

# define	LOG_NB_LEVEL	5	/* number of availaible logs level */

# define LOG_ERROR(_level, _depth, _message, ...) {logs_error(_level, _depth, "%s:%s "##_message, __func__, __LINE__, __VA_ARGS__)}
# define LOG_PERROR(_level, _depth, _message, ...) {logs_perror(_level, _depth, "%s:%s "##_message, __func__, __LINE__, __VA_ARGS__)}

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

# define	logs(var, ...) (_Generic((var),				\
	t_log_report *:			_logs_report_adapter,		\
	const t_log_report *:	_logs_report_adapter,		\
	default:				logs_raw					\
)(var, __func__, __FILE__, __LINE__, ## __VA_ARGS__))

# define	logs_perror(var, ...) (_Generic((var),			\
	t_log_report *:			_logs_perror_report_adapter,	\
	const t_log_report *:	_logs_perror_report_adapter,	\
	default:				logs_perror_raw			\
)(var, __func__, __FILE__, __LINE__, ## __VA_ARGS__))

int	log_init(
	const t_log_init *const _data
);

/** */
int	logs_raw(
	const t_log_level level,
	const char *const func,
	const char *const file,
	const int line,
	const char *const summary,
	const char *const body, ...
);

/** */
int	logs_perror_raw(
	const t_log_level level,
	const char *const func,
	const char *const file,
	const int line,
	const char *const summary,
	const char *const body, ...
);

/** */
int	logs_report(
	const t_log_report *const report
);

/** */
int	logs_perror_report(
	const t_log_report *const report
);

static inline int	_logs_report_adapter(
	const t_log_report *const report,
	...
)
{
	return (logs_report(report));
}

static inline int	_logs_perror_report_adapter(
	const t_log_report *const report,
	...
)
{
	return (logs_perror_report(report));
}

#endif	// LIB_INTERFACE_LOG_H
