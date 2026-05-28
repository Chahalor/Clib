/**
 * 
*/

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
// # include "config.h"
// # include "standards.h"

/* -----| Internals |----- */
# include "types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

#define ALLOC_SIZE	256

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

int	array_alloc(
	t_array *const	array,
	uint32_t		capacity
);

void		array_free(
				t_array *const	array,
				const int		all
			);

int	array_append(
	t_array *const	array,
	void *const		data
);

t_module	*module_new(void);

/** */
int	setup(
	Config *const config
);