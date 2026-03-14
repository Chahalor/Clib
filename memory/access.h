// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
# include <stddef.h>

/* -----| Globals   |----- */
# include "lib_config.h"
# include "lib_standards.h"

/* -----| Internals |----- */
// # include "types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

typedef struct s_mem_node	t_mem_node;

struct s_mem_node
{
	size_t	capacity;
	void	*data;
};

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

#ifdef MEM_SAFE_ACCESS
# define access(ptr, index) {t_mem_node *_node = (t_mem_node *)(ptr - sizeof(size_t)); _node->capacity < index ? _node->data[index] : _node->data[_node->capacity]}
#else
# define access(ptr, index) (ptr[index]);
#endif

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */
	//...