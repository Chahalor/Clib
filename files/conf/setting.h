/**
 * 
*/

#ifndef CONF_SETTING_H
# define CONF_SETTING_H

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
# include "../conf.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Types                                      */
/* ************************************************************************** */

typedef void *(*t_alloc_fn)(size_t);
typedef void *(*t_realloc_fn)(void*, size_t);
typedef void *(*t_free_fn)(void *);

/* ************************************************************************** */
/*                                 Functions                                  */
/* ************************************************************************** */

//--- Setter ---//

void	conf_set_allocator(
	t_alloc_fn func
);

void	conf_set_reallocator(
	t_realloc_fn func
);

void	conf_set_free(
	t_free_fn func
);

//--- Getter ---//

const t_alloc_fn	conf_get_allocator(void);
const t_realloc_fn	conf_get_reallocator(void);
const t_free_fn		conf_get_free(void);

#endif	// CONF_SETTING_H