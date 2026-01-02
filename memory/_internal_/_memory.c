// Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* Internal */
#include "_memory.h"
	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

#if MEM_MANAGER_TYPE == 0

int	_hash(
	const void *const _ptr
)
{
	return ((int)((size_t)_ptr % MEM_BUCKET_SIZE));
}

t_mem_node	*_mem_find_node(
	const t_mem_manager *const _manager,
	const void *const _ptr
)
{
	if (unlikely(!_manager || !_ptr))
		return (NULL);

	const int	_id = _hash(_ptr);
	t_mem_node	*_node;

	_node = _manager->bucket[_id];
	while (_node && _node->content != _ptr)
		_node = _node->next;
	return (_node);
}

int	_mem_register_node(
	t_mem_manager *const _manager,
	t_mem_node *_node
)
{
	const int	_id = _hash(_node->content);
	t_mem_node	*_current;

	if (unlikely(!_manager || !_node))
		return (error_invalid_arg);

	_current = _manager->bucket[_id];
	_node->next = _current;
	_manager->bucket[_id] = _node;
	return (mem_manager_error_none);
}

t_mem_node	*_mem_new_node(
	const size_t _size
)
{
	t_mem_node *result = NULL;

	result = calloc(1, _size + sizeof(t_mem_node));
	if (unlikely(!result))
		return (NULL);
	result->content = (void *)(result + 1);
	result->size = _size;
	result->next = NULL;
	return (result);
}

int	_mem_unregister_node(
	t_mem_manager *const _manager,
	t_mem_node *_node,
	const int _must_free
)
{
	const int	_id = _hash(_node->content);
	t_mem_node	*_current;
	t_mem_node	*_previous;

	if (unlikely(!_manager || !_node))
		return (error_invalid_arg);

	_current = _manager->bucket[_id];
	if (unlikely(!_current))
		return (mem_manager_error_unregister_failed);
	while (_current && _current->next && _current != _node)
	{
		_previous = _current;
		_current = _current->next;
	}
	if (_previous)
		_previous->next = _current->next;
	else
		_manager->bucket[_id] = _current->next;
	if (_must_free)
		free(_current);
	return (mem_manager_error_none);
}

/**
 * @brief	set *_output to a pointer to a newly allocated area and register the node into the manager
 * 
 * @param	_manager	the manager data struct
 * @param	_size		the allocation size
 * @param	_output		the output pointer
 * 
 * @return	the error code
 * 
 * @visibility	local
 * 
 * @version 2.0.0
*/
int	_mem_alloc(
	t_mem_manager *const _manager,
	const size_t _size,
	void **_output
)
{
	t_mem_node	*_node = NULL;

	_node = _mem_new_node(_size);
	if (unlikely(!_node))
	{
		*_output = NULL;
		return (mem_manager_error_alloc_failed);
	}

	_mem_register_node(_manager, _node);
	*_output = _node->content;
	return (mem_manager_error_none);
}

/**
 * @brief	reallocat _ptr to _size and register/unregister pointer.
 * 
 * @param	_manager	the manager data struct
 * @param	_ptr		the pointer to be reallocated
 * @param	_size		the allocation size
 * @param	_output		the output pointer
 * 
 * @return	the error code
 * 
 * @visibility	local
 * 
 * @version 2.0.0
*/
int	_mem_realloc(
	t_mem_manager *_manager,
	void *_ptr,
	const size_t _size,
	void **_output
)
{
	t_mem_node	*_node = _mem_find_node(_manager, _ptr);
	size_t		_dummy = (size_t)_node;
	t_mem_node	*_new = NULL;

	if (unlikely(!_node))
		return (_mem_manager(mem_manager_access_alloc, _output, _size));
	_new = realloc(_node, sizeof(t_mem_node) + _size);
	if (unlikely(!_new))
	{
		*_output = NULL;
		return (mem_manager_error_alloc_failed);
	}
	if (_dummy != (size_t)_new)
	{
		_mem_unregister_node(_manager, (void *)_dummy, 0);
		_mem_register_node(_manager, _new);
	}
	*_output = _new->content;
	return (mem_manager_error_none);
}

/**
 * @brief	duplicate _ptr based on _size or the stored size
 * 
 * @param	_manager	the manager data struct
 * @param	_ptr		the pointer to be reallocated
 * @param	_size		the allocation size
 * @param	_output		the output pointer
 * 
 * @return	the error code
 * 
 * @visibility	local
 * 
 * @version 2.0.0
*/
int	_mem_dup(
	t_mem_manager *const _manager,
	void *const _ptr,
	const ssize_t _size,
	void **_output
)
{
	t_mem_node	*_target = NULL;
	t_mem_node	*_new = NULL;

	if (_size < 0)
	{
		_target = _mem_find_node(_manager, _ptr);
		if (unlikely(!_target))
		{
			*_output = NULL;
			return (mem_manager_error_node_not_found);
		}
		_new = _mem_new_node(_target->size);
		if (unlikely(!_new))
		{
			*_output = NULL;
			return (error_alloc_fail);
		}
		memcpy(_new->content, _target->content, _new->size);
	}
	else
	{
		_new = _mem_new_node(_size);
		if (unlikely(!_new))
		{
			*_output = NULL;
			return (error_alloc_fail);
		}
		memcpy(_new->content, _ptr, _size);
	}
	_mem_register_node(_manager, _new);
	*_output = _new->content;
	return (mem_manager_error_none);
}

/**
 * @brief	free _ptr and remove the linked node from the tree
 * 
 * @param	_manager	the manager data struct
 * @param	_ptr		the pointer to be reallocated
 * 
 * @return	the error code
 * 
 * @visibility	local
 * 
 * @version 2.0.0
*/
int	_mem_free(
	t_mem_manager *const _manager,
	void *_target
)
{
	t_mem_node	*_node = _mem_find_node(_manager, _target);

	if (unlikely(!_node))
		return (mem_manager_error_unregister_failed);
	else
		return (_mem_unregister_node(_manager, _node, 1));
}

/**
 * @brief	free all the stored memory
 * 
 * @param	_manager	the manager data struct
 * 
 * @return	the error code
 * 
 * @visibility	local
 * 
 * @version 2.0.0
*/
int	_mem_free_all(
	t_mem_manager *const _manager
)
{
	t_mem_node	*_current;
	t_mem_node	*_next;
	int			_i = 0;

	while (_i < MEM_BUCKET_SIZE)
	{
		_current = _manager->bucket[_i];
		while (_current && _current->next)
		{
			_next = _current->next;
			free(_current);
			_current = _next;
		}
		free(_current);
		_manager->bucket[_i] = NULL;
		_i++;
	}
	return (mem_manager_error_none);
}

# elif MEM_MANAGER_TYPE == 1

/**
 * @brief	set *_output to a pointer to a newly allocated area and register the node into the manager
 * 
 * @param	_manager	the manager data struct
 * @param	_size		the allocation size
 * @param	_output		the output pointer
 * 
 * @return	the error code
 * 
 * @visibility	local
 * 
 * @version 2.0.0
*/
int	_mem_alloc(
	t_mem_manager *const _manager,
	const size_t _size,
	void **_output
)
{
	rb_node	*_node = NULL;

	_node = rb_create_node(_manager->T, _size);
	if (unlikely(!_node))
	{
		*_output = NULL;
		return (mem_manager_error_alloc_failed);
	}

	rb_insert(_manager->T, _node);
	*_output = _node->key;
	return (mem_manager_error_none);
}

/**
 * @brief	reallocat _ptr to _size and register/unregister pointer.
 * 
 * @param	_manager	the manager data struct
 * @param	_ptr		the pointer to be reallocated
 * @param	_size		the allocation size
 * @param	_output		the output pointer
 * 
 * @return	the error code
 * 
 * @visibility	local
 * 
 * @version 2.0.0
*/
int	_mem_realloc(
	t_mem_manager *_manager,
	void *_ptr,
	const size_t _size,
	void **_output
)
{
	rb_node		*_node = rb_search(_manager->T, _ptr);
	rb_node		*_new = NULL;

	if (unlikely(!_node))
		return (_mem_manager(mem_manager_access_alloc, _output, _size));
	_new = rb_create_node(_manager->T, _size);
	if (unlikely(!_new))
	{
		*_output = NULL;
		return (mem_manager_error_alloc_failed);
	}
	memcpy(_new->key, _node->key, min(_node->size, _size));
	rb_insert(_manager->T, _new);
	rb_delete(_manager->T, _node, true);
	return (error_none);
}

/**
 * @brief	duplicate _ptr based on _size or the stored size
 * 
 * @param	_manager	the manager data struct
 * @param	_ptr		the pointer to be reallocated
 * @param	_size		the allocation size
 * @param	_output		the output pointer
 * 
 * @return	the error code
 * 
 * @visibility	local
 * 
 * @version 2.0.0
*/
int	_mem_dup(
	t_mem_manager *const _manager,
	void *const _ptr,
	const ssize_t _size,
	void **_output
)
{
	rb_node			*_target = NULL;
	rb_node			*_new = NULL;

	if (_size < 0)
	{
		_target = rb_search(_manager->T, _ptr);
		if (unlikely(!_target))
		{
			*_output = NULL;
			return (mem_manager_error_node_not_found);
		}
		_new = rb_create_node(_manager->T, _target->size);
		if (unlikely(!_new))
		{
			*_output = NULL;
			return (error_alloc_fail);
		}
		memcpy(_new->key, _target->key, _new->size);
	}
	else
	{
		_new = rb_create_node(_manager->T, _size);
		if (unlikely(!_new))
		{
			*_output = NULL;
			return (error_alloc_fail);
		}
		memcpy(_new->key, _ptr, _size);
	}
	rb_insert(_manager->T, _new);
	*_output = _new->key;
	return (mem_manager_error_none);
}

/**
 * @brief	will join two memory area into one new one
 * 
 * if any size is negative, it will use the manager stored size instead
 * 
 * @param	manager	the memory manager
 * @param	ptr1	the first pointer to be joined
 * @param	ptr2	the seconde pointer to be joined
 * @param	size1	the size of the first memory area
 * @param	size2	the size of the second memory area
 * @param	result	the pointer for the resulted allocation
 * 
 * @return	error_none or the corresponding error code
 * @retval		`error_none`: no error
 * @retval		`error_invalid_arg`: invalid size with NULL pointer
 * @retval		all `_mem_alloc()` errors
 * 
 * @version	1.0.0
*/
int	_mem_join(
	t_mem_manager *const _manager,
	const void *const _ptr1,
	const void *const _ptr2,
	const ssize_t _size1,
	const ssize_t _size2,
	void **const result
)
{
	const rb_node	*_node1 = rb_search(_manager->T, _ptr1);
	const rb_node	*_node2 = rb_search(_manager->T, _ptr2);;
	size_t			_s1 = 0;
	size_t			_s2 = 0;
	void			*_dummy = NULL;
	int				error = error_none;

	if (unlikely((!_node1 && _size1 < 0) || (!_node2 && _size2 < 0)))
	{
		error = error_invalid_arg;
		goto cleannup;
	}

	if (_size1 < 0)
		_s1 = _node1->size;
	else
		_s1 = _size1;
	if (_size2 < 0)
		_s2 = _node2->size;
	else
		_s2 = _size2;

	error = _mem_alloc(_manager, _s1 + _s2, &_dummy);
	if (unlikely(error))
		goto cleannup;

	memcpy(_dummy, _ptr1, _s1);
	memcpy(_dummy + _s1, _ptr2, _s2);
	*result = _dummy;

cleannup:
	return (error);
}

/**
 * @brief	free _ptr and remove the linked node from the tree
 * 
 * @param	_manager	the manager data struct
 * @param	_ptr		the pointer to be reallocated
 * 
 * @return	the error code
 * 
 * @visibility	local
 * 
 * @version 2.0.0
*/
int	_mem_free(
	t_mem_manager *const _manager,
	void *_target
)
{
	rb_node	*_node = rb_search(_manager->T, _target); // _mem_find_node(_manager, _target);

	if (unlikely(!_node))
		return (mem_manager_error_unregister_failed);
	else
		return (rb_delete(_manager->T, _node, true), error_none);
		// return (_mem_unregister_node(_manager, _node, 1));
}

/**
 * @brief	recursivly free the rb tree
 * 
 * @param	_node	the current node to free
 * 
 * @return	void
 * 
 * @visibility	local
 * 
 * @version 1.0.0
*/
static void	_mem_free_all_recur(
	rb_tree *T,
	rb_node *_node
)
{
	if (unlikely(_node == T->nil))
		return ;

	_mem_free_all_recur(T, _node->left);
	_mem_free_all_recur(T, _node->right);
	free(_node);
}

/**
 * @brief	free all the stored memory
 * 
 * @param	_manager	the manager data struct
 * 
 * @return	the error code
 * 
 * @visibility	local
 * 
 * @version 2.0.0
*/
int	_mem_free_all(
	t_mem_manager *const _manager
)
{
	if (unlikely(!_manager->T))
		return (error_invalid_arg);

	_mem_free_all_recur(_manager->T, _manager->T->root);
	free(_manager->T->nil);
	free(_manager->T);
	_manager->T = NULL;
	return (error_none);
}

#else

#error "MEM_MANAGER_TYPE is invalide"

#endif

/* ----| Public     |----- */

/**
 * @brief	manager all the memory access for allocation, reallocation and free
 * 
 * @param	_access	the type of access
 * @param	_output	the pointer to the output value
 * 
 * @return	the error code
 * 
 * @visibility	local
 * 
 * @version 2.0.0
*/
int	_mem_manager(
	const int _access,
	void *_output,
	...
)
{
	static t_mem_manager	manager = {0};
	int						out;
	va_list					_list;

	#if MEM_MANAGER_TYPE == 1

	if (unlikely(!manager.T))
		manager.T = rb_init();
	
	#endif

	va_start(_list, _output);
	switch (_access)
	{
		case (mem_manager_access_alloc):
		{
			size_t	_size = va_arg(_list, size_t);

			out = _mem_alloc(&manager, _size, (void **)_output);
			break;
		}
		case (mem_manager_access_realloc):
		{
			void	*_ptr = va_arg(_list, void *);
			size_t	_size = va_arg(_list, size_t);

			out = _mem_realloc(&manager, _ptr, _size, (void **)_output);
			break;
		}
		case (mem_manager_access_join):
		{
			const void *const _ptr1 = va_arg(_list, void *);
			const void *const _ptr2  = va_arg(_list, void *);
			const ssize_t _size1 = va_arg(_list, ssize_t);
			const ssize_t _size2 = va_arg(_list, ssize_t);

			out = _mem_join(&manager, _ptr1, _ptr2, _size1, _size2, _output);
			break;
		}
		case (mem_manager_access_dup):
		{
			void	*_ptr = va_arg(_list, void *);
			ssize_t	_size = va_arg(_list, ssize_t);

			out = _mem_dup(&manager, _ptr, _size, (void **)_output);
			break ;
		}
		case (mem_manager_access_free):
		{
			void	*_target = va_arg(_list, void *);

			out = _mem_free(&manager, _target);
			break;
		}
		case (mem_manager_access_free_all):
			out = _mem_free_all(&manager);
			break;
		default:
			out = error_invalid_arg;
	}

	va_end(_list);
	return (out);
}
