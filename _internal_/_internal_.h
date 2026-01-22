// Header

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
# include "_types_.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/* -----| Memory   |----- */
#pragma region Memory

__attribute__((visibility("hidden")))
void	*_mem_alloc_(
	const size_t size
);

__attribute__((visibility("hidden")))
void	_mem_free_(
	void *const restrict ptr
);

/* -----| Strings  |----- */

/** */
__attribute__((visibility("hidden")))
_t_string_	*_string_new_(
	size_t length
);

/** */
__attribute__((visibility("hidden")))
int	_string_append(
	_t_string_ *const target,
	const char *const src
);

/** */
__attribute__((visibility("hidden")))
void	_string_free_(
	_t_string_ *const target
);
