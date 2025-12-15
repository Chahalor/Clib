// Header

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

void	*mem_alloc(
			const size_t _size
			);

void	*mem_realloc(
			void *_ptr,
			const size_t _size
			);

void	mem_free(
			void *const restrict _ptr
			);

void	*mem_dup(
		const void *const restrict _ptr,
		const ssize_t _size
		);

void	mem_free_all(void);