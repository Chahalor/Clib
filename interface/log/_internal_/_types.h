/**
 * @file _types.h
 * @brief Internal types for `interface/log`.
 *
 * This header stores module-private runtime structures used by internal
 * configuration and write paths.
 */

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stdarg.h>

/* -----| Globals   |----- */
	//...

/* -----| Internals |----- */
#include "../../log.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */

typedef struct s_log_internal	t_log_internal;
typedef struct s_log_write		t_log_write;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

enum e_log_manager_access
{
	e_log_mng_init,		/**< Initialize log manager state. */
	e_log_mng_write,	/**< Standard write path. */
	e_log_mng_perror,	/**< Write path with `errno` text. */
	e_log_mng_error,	/**< Reserved error-management action. */
	e_log_mng_close,	/**< Close one stream. */
	e_log_mng_close_all,/**< Close all streams. */
};

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

struct s_log_internal
{
	int				fd[LOG_NB_LEVEL];		/**< Per-level destination file descriptors. */
	unsigned int	logs[LOG_NB_LEVEL];		/**< Per-level write counters (reserved). */
	int				last_depth[LOG_NB_LEVEL];/**< Per-level last depth (reserved). */
	int				display_limit;			/**< Terminal display threshold. */
	int				is_init;				/**< Non-zero once `_log_init()` ran. */
};

/**
 * @brief Legacy write request container used by old paths.
 */
struct s_log_write
{
	int		level;	/**< Target log level. */
	int		depth;	/**< Indentation depth. */
	char	*format;/**< Format string. */
	va_list	args;	/**< Arguments for formatted payload. */
};
