/**
 * 
*/

#ifndef	LIB_INTERFACE_LOG_INTERNAL_H
# define LIB_INTERFACE_LOG_INTERNAL_H

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "lib_config.h"
# include "lib_standards.h"

/* -----| Internals |----- */
# include "_types.h"
#include "../../log.h"

/* -----| Modules   |----- */
#include "lib.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

int	_log_init(
	t_log_internal *const config,
	const t_log_init *const data
);

t_log_internal	*_log_config(
	const t_log_init *const init
);

int	_logs_print(
	const t_log_report *report,
	const int fd,
	const int depth
);

int	_logs_print_atomic(
	const t_log_report *report,
	const int fd
);

int	_logs(
	t_log_report *const report,
	const bool atomic_safe,
	const bool perr
);

#endif	// LIB_INTERFACE_LOG_INTERNAL_H