/**
 * @file memory.h
 * @brief API public to the memory manager
 *
 * this module provide many fonction to manager more safely the memory.
 *
 * @ingroup memory
 */

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stddef.h>
#include <sys/types.h>

/* -----| Globals   |----- */
# include "lib_config.h"
# include "lib_standards.h"

/* -----| Internals |----- */
# include "types.h"

/* -----| Modules   |----- */
#include "../math/math.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/**
 * @brief	allocate `size` bit of memory using `malloc`
 * 
 * register the allocated memory to the memory manager
 * 
 * @param	size	the memory size to be allocated
 * 
 * @return	the memory allocated or NULL in case of malloc errors or invalid allocation size
 * 
 * @example	char	*str = mem_alloc(sizeof(char) * strlen(input));
 * 
 * @version	1.1.0
 */
void	*mem_alloc(
			const size_t _size
			);

/**
 * @brief	reallocate `ptr` to the new size `size`
 * 
 * @param	ptr		the memory area to be reallocated
 * @param	size	the new size of the memory area
 * 
 * @return	the pointer to the new area or NULL in case of allocation error
 * 
 * @version	1.1.0
*/
void	*mem_realloc(
			void *_ptr,
			const size_t _size
			);

/**
 * @brief	free `ptr` from the memory manager and from the programm scope
 * 
 * @param	ptr	the pointer to be freed
 * 
 * @return	void
 * 
 * @version	1.1.0
*/
void	mem_free(
			void *const restrict _ptr
			);

/**
 * @brief	duplicate `size` bit from `ptr` to a newly allocated memory area.
 * 
 * if `size` < 0, use the memory manager data to detemine `ptr` allocation size
 * 
 * @param	ptr		the pointer to be duplicated
 * @param	size	the number of bit to be duplicate
 * 
 * @return	a pointer to the new area or NULL in case of error
 * 
 * @version	1.0.0
 */
void	*mem_dup(
			const void *const restrict _ptr,
			const ssize_t _size
			);

/**
 * @brief	free `all` the memory allocated and stored inside of the memory manager
 * 
 * @param	none
 * 
 * @return	void
 * 
 * @version	1.0.0
*/
void	mem_free_all(void);