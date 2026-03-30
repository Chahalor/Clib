/**
 * @author	@Chahalor
 * @file	types.h
 * @brief	Public types for the `interface/log` module.
 *
 * This file defines log levels, error aliases, and the public structures used
 * by `log_init()` and logging report calls.
 * 
 * @date	30/03/2026
 */

#ifndef LIB_INTERFACE_LOG_TYPES_H
# define LIB_INTERFACE_LOG_TYPES_H

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
# include <sys/time.h>

/* -----| Globals   |----- */
# include "../../standards/lib_standards.h"

/* -----| Internals |----- */
	//...

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */
/* -----| Typename  |----- */
	//...

/* -----| Enums     |----- */

typedef enum e_log_level	t_log_level;	//

/* -----| Unions    |----- */
	//...

/* -----| Structs   |----- */

typedef struct s_log_file	t_log_file;
typedef struct s_log_init	t_log_init;
typedef struct s_log_report	t_log_report;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

/**
 * @brief Logging levels handled by the module.
 * 
 * @enum	log_level
 */
enum e_log_level
{
	log_error,	/**< Error-level log. */
	log_warning,/**< Warning-level log. */
	log_info,	/**< Informational log. */
	log_debug,	/**< Debug-level log. */
	log_other	/**< Fallback level for unknown/other events. */
};

/**
 * @brief Log-module error code aliases.
 *
 * These values are currently declared for module-level semantic clarity.
 * 
 * @enum	log_errors
 */
enum e_log_errors
{
	log_error_none = error_none,	/**< No error. */
	log_error_invalid_logfile,		/**< Invalid or unusable log file setup. */
	log_error_write_impossible,		/**< Write path could not complete. */
};


/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

/**
 * @brief One output stream configuration.
 *
 * `filename == NULL` means fallback to `STDERR_FILENO` in current setup code.
 */
struct s_log_file
{
	char	*filename;	/**< Path to the log file, or NULL for stderr fallback. */
	int		mode;		/**< Reserved open mode field (currently unused by setup). */
	int		perms;		/**< Reserved permission field (currently unused by setup). */
	int		fd_file;	/**< Reserved descriptor cache field. */
	int		fd_tty;		/**< Reserved terminal descriptor cache field. */
};

/**
 * @brief Global logging initialization payload.
 *
 * One `t_log_file` is provided per level. `display_limit` controls whether a
 * log also goes to terminal output:
 * `_fd_tty = (display_limit > level) ? -1 : STDERR_FILENO`, with `log_info`
 * redirected to `STDOUT_FILENO`.
 */
struct s_log_init
{
	t_log_file	error;			/**< Configuration for `log_error` stream. */
	t_log_file	warning;		/**< Configuration for `log_warning` stream. */
	t_log_file	info;			/**< Configuration for `log_info` stream. */
	t_log_file	debug;			/**< Configuration for `log_debug` stream. */
	t_log_file	other;			/**< Configuration for `log_other` stream. */
	int			display_limit;	/**< Terminal display threshold. */
};

/**
 * @brief Log entry payload used by report-based logging calls.
 *
 * `sub` can chain child reports to produce an indented tree output.
 *
 * @version 2.0.0
 *
 * @code{.c}
 * t_log_report child = {
 *     .level = log_debug,
 *     .summary = "sql trace",
 *     .body = "SELECT * FROM users WHERE id=42"
 * };
 * t_log_report root = {
 *     .level = log_info,
 *     .summary = "request finished",
 *     .body = "status=200",
 *     .sub = &child
 * };
 * logs_report(&root);
 * @endcode
 */
struct s_log_report
{
	char				*summary;	/**< One-line summary. */
	char				*body;		/**< Optional multi-line details. */
	t_log_report		*sub;		/**< Optional child report (recursive). */
	const char			*func;		/**< Caller function label. */
	const char			*file;		/**< Caller source file label. */
	int					line;		/**< Caller source line. */
	time_t				time;		/**< Explicit timestamp, or 0 for now. */
	enum e_log_level	level;		/**< Severity level. */
	int					code;		/**< Application/system code displayed in summary. */
};

#endif	// LIB_INTERFACE_LOG_TYPES_H
