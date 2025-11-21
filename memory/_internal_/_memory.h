// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stdlib.h>

/* -----| Globals   |----- */
# include "config.h"
# include "standards.h"

/* -----| Internals |----- */
# include "_types.h"
# include "../memory.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/* -----| rb tree |----- */

#if MEM_MANAGER_TYPE == 1

rb_tree	*rb_init(void);

rb_node	*rb_create_node(
			rb_tree *T,
			size_t size
			);

void	rb_insert(
			rb_tree *T,
			rb_node *z
			);

rb_node	*rb_search(
			rb_tree *T,
			void *key
			);

void	rb_delete(
			rb_tree *T,
			rb_node *z,
			const int _free
			);

#endif

int		_mem_manager(
			const int _access,
			void *_output,
			...
			);
