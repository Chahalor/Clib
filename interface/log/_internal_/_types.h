// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stdarg.h>

/* -----| Globals   |----- */
	//...

/* -----| Internals |----- */
#include "../log.h"

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
	e_log_mng_init,
	e_log_mng_write,
	e_log_mng_perror,	// glic error
	e_log_mng_error,	// mein error
	e_log_mng_close,
	e_log_mng_close_all,
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
	int				fd[LOG_NB_LEVEL];			/* every log io                                */
	unsigned int	logs[LOG_NB_LEVEL];			/* number of log write in every log io        */
	int				last_depth[LOG_NB_LEVEL];	/* lastd epth for every logfile              */
	int				display_limit;				/* the max level to be displayed to the cmd */
	int				is_init;					/* check if the logs get init at some point*/
};

struct s_log_write
{
	int		level;
	int		depth;
	char	*format;
	va_list	args;
};
