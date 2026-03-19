/**
 * 
*/

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>

	/* Internal */
#include "_conf.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

extern _t_conf_allocator	_g_allocator = (_t_conf_allocator)
{
	.alloc = mem_alloc,
	.realloc = mem_realloc,
	.free = mem_free
};

_t_conf_node	*_conf_mem_new_node(
	const char *const key,
	_t_conf_value value
)
{
	size_t			_alloc_size = sizeof(_t_conf_node) + strlen(key);
	_t_conf_node	*result = NULL;

	result = _g_allocator.alloc(_alloc_size);
	if (unlikely(!result))
		return (NULL);
	result->value = value;
	result->key = (char *)(result + 1);
	strcpy(result->key, key);
	result->child = NULL;
	result->next = NULL;

	return (result);
}

void	_conf_mem_add_next(
	_t_conf_node *const dest,
	_t_conf_node *const node
)
{
	_t_conf_node	*_dest = dest;

	if (!_dest->next)
	{
		_dest->next = node;
		return ;
	}

	for (;
		_dest->next != NULL;
		_dest = _dest->next
	)
	{
		;
	}

	_dest->next = node;
}

void	_conf_mem_add_child(
	_t_conf_node *const dest,
	_t_conf_node *const node
)
{
	_t_conf_node	*_dest = dest;

	if (!_dest->child)
	{
		_dest->child = node;
		return ;
	}

	for (;
		_dest->child != NULL;
		_dest = _dest->child
	)
	{
		;
	}

	_dest->child = node;
}