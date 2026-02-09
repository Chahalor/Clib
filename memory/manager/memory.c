// Header

/* ----| Headers    |----- */
	/* Standard */
		//...
	/* Internal */
#include "_internal_/_memory.h"
#include "memory.h"
	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

/**
 * @brief	allocate _size bit of memory (is just a malloc wrapper + garbage collector)
 * 
 * @param	_size	the allocation size
 * 
 * @return	the pointer to the area or NULL in case of error
 * 
 * @visbility	public
 * 
 * @version 1.2.0
 */
void	*mem_alloc(
	const size_t _size
)
{
	void	*result = NULL;
	int		_error = error_none;

	_error = _mem_manager(mem_manager_access_alloc, &result, _size);
	if (likely(_error == error_none))
		return (result);
	return (NULL);
}

/**
 * @brief	reallocate _area to _size
 * 
 * @param	_ptr	the memory area to reallocate
 * @param	_size	the new size of the area
 * 
 * @return	the extended pointer or NULL in case of error
 * 
 * @visibility	public
 * 
 * @version 1.1.0
*/
void	*mem_realloc(
	void *_ptr,
	const size_t _size
)
{
	void	*result = NULL;
	int		_error = error_none;

	_error = _mem_manager(mem_manager_access_realloc, &result, _ptr, _size);
	if (likely(_error == error_none))
		return (result);
	return (NULL);
}

/**
 * @brief	duplicate the memory inside _ptr. if _size < 0 use the size stored by the manager
 * 
 * @param	_ptr	the memory area to reallocate
 * @param	_size	the new size of the area
 * 
 * @return	the newly allocated pointer or NULL in case of error
 * 
 * @visibility	public
 * 
 * @version 1.0.0
*/
void	*mem_dup(
	const void *const restrict _ptr,
	const ssize_t _size
)
{
	void	*result = NULL;
	int		_error = error_none;

	if (unlikely(!_ptr))
		_error = _mem_manager(mem_manager_access_alloc, &result, _size);
	else
		_error = _mem_manager(mem_manager_access_dup, &result, _ptr, _size);
	(void)_error;
	return (result);
}

void	*mem_join(
	const void *const restrict _ptr1,
	const void *const restrict _ptr2,
	const ssize_t _size1,
	const ssize_t _size2
)
{
	int		_error = error_none;
	void	*result = NULL;

	if (unlikely(!_ptr1 || !_ptr2))
		return (NULL);

	_error = _mem_manager(mem_manager_access_join, &result, _ptr1, _ptr2, _size1, _size2);
	(void)_error;

	return (result);
}

/**
 * @brief	free _area
 * 
 * @param	_ptr	the memory area to free
 * 
 * @return	void
 * 
 * @visibility	public
 * 
 * @version 1.0.0
*/
void	mem_free(
	void *const restrict _ptr
)
{
	int	_error = error_none;

	_error = _mem_manager(mem_manager_access_free, NULL, &_ptr);
	(void)_error;
}

/**
 * @brief	free all allocated memory by the program
 * 
 * @param	void
 * 
 * @return	void
 * 
 * @visibility	public
 * 
 * @version 1.0.0
*/
__attribute__((destructor))
void	mem_free_all(void)
{
	_mem_manager(mem_manager_access_free_all, NULL);
}
