/**
 * @file toml.h
 * @brief Public API for the TOML module.
 */
#ifndef CLIB_FILES_TOML_H
# define CLIB_FILES_TOML_H

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
# include <stdio.h>
# include <stdarg.h>
# include <stdint.h>

/* -----| Globals   |----- */
# include "../standards/lib_config.h"
# include "../standards/lib_standards.h"

/* -----| Internals |----- */
# include "toml/types.h"
# include "toml/_internal_/_types.h"

/* -----| Modules   |----- */
# include "../memory.h"
# include "../processor/processor.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */
# define toml_foreach(var, node)												\
	for (TOML *(var) = ((TOML *)(node) ? ((TOML *)(node))->child : NULL);	\
		(var) != NULL;														\
		(var) = (var)->next)

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

TOML	*toml_new(void);

TOML	*toml_load(
			const char *const restrict path
			);

TOML	*toml_load_str(
			const char *const restrict format,
			...
			);

int		toml_unload(
			TOML *toml
			);

void	toml_free(
			TOML *const restrict obj
			);

void	*toml_get(
			TOML *const obj,
			const char *const restrict key,
			...
			);

size_t	toml_sizeof(
			const TOML *const restrict toml
			);

size_t	toml_len(
			const TOML *const restrict toml
			);

/**
 * @brief	set the field of the toml element with _value
 * 
 * @param	toml	the toml element
 * @param	field	the field to be set
 * @param	value	the value to be writen
 * 
 * @return	the errnum of the action
 * 
 * @example toml_set(toml, "data.key.git.user1", var, toml_str);
 * @example toml_set(toml, "data.key.git.%s", var, toml_str, "user1");
 * @example toml_set(toml, "data.key.git.user1", "bob is %s", toml_str, "bob");
 * 
 * @version	3.0.0
*/
#define toml_set(toml, field, var, ...) \
	__builtin_choose_expr( \
		_is_number(var), \
		__builtin_choose_expr( \
			_is_signed(var), \
			toml_set_signed_nb, \
			toml_set_unsigned_nb \
		), \
		__builtin_choose_expr( \
			_is_string(var), \
			toml_set_string, \
			toml_set_wild \
		) \
	)(toml, field, var, ##__VA_ARGS__)

int	toml_set_signed_nb(
	TOML *const toml,
	const char *const field,
	signed long long var, ...
);

int	toml_set_unsigned_nb(
	TOML *const toml,
	const char *const field,
	unsigned long long var, ...
);

int	toml_set_string(
	TOML *const toml,
	const char *const field,
	char *var, ...
);

int	toml_set_obj(
	TOML *const toml,
	const char *const field,
	TOML *var, ...
);

int	toml_set_array(
	TOML *const toml,
	const char *const field,
	void *var, ...
);

int	toml_set_wild(
	TOML *const toml,
	const char *const field,
	void *var,
	int type, ...
);

int		toml_set_from_array(
			TOML *toml,
			const char *const restrict field,
			void *const array,
			const size_t length,
			const int type,
			...
			);

int		toml_array_append(
			TOML *const toml,
			void *const value,
			const int type
			);

int		toml_array_pop(
			TOML *const toml,
			const size_t index
			);

int		toml_unset(
			TOML *toml,
			const char *const restrict field,
			...
			);

int		toml_remove(
			TOML *toml,
			const char *const restrict field,
			...
			);

int		toml_dump(
			TOML *const restrict toml,
			FILE *const file,
			const int pretty
			);

char	*toml_stringify(
			TOML *toml,
			const int pretty
			);

int		toml_assert_type(
			const TOML *const node,
			const int type
			);

int		toml_get_type(
			const TOML *const node
			);

#endif	// CLIB_FILES_TOML_H
