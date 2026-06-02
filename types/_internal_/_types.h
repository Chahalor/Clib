/**
 * 
*/
#ifndef TYPES_INTERNAL_TYPES_H
# define TYPES_INTERNAL_TYPES_H
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
# include "../types.h"
# include "../setting.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

# define	setting (&g_types_settings)
# define	return_error(_errnum, _key, _return)	\
do													\
{													\
	setting->error.errnum = _errnum;				\
	setting->error.key = _key;						\
	return (_return);								\
} while (0);


/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */
	//...

#endif	// TYPES_INTERNAL_TYPES_H