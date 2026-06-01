/**
 * 
*/
#ifndef CLIB_TYPE_H
# define CLIB_TYPE_H
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
# include "types/types.h"

/* -----| Modules   |----- */
# include "processor/processor.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

#define	ARRAY_ACCESS(type, array, index)									\
	__builtin_choose_expr(													\
		IS_TYPE(typeof(array), t_array),									\
		(*(type *)((char *)(array).data + (array).elt_size * (index))),		\
		(*(type *)((char *)(array)->data + (array)->elt_size * (index)))	\
	)

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

int	array_alloc(
	t_array *const	array,
	const size_t	capacity
);

void	array_free(
	t_array *const	array
);

int		array_append(
	t_array *const	array,
	void *const		data
);

void	array_pop(
	t_array *const	array,
	const bool		free
);

#endif	// CLIB_TYPE_H