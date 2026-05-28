/**
 * @file _toml.h
 * @brief Internal TOML helpers.
*/
#ifndef CLIB_FILES_TOML_INTERNAL_H
# define CLIB_FILES_TOML_INTERNAL_H

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
# include "_types.h"
# include "../../toml.h"
#include "../setting.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

# define setting (&g_toml_settings)

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

TOML	*_toml_new_content(
			const char *const key,
			const int type,
			char *const data
			);

void	_toml_free_content(
			TOML *node
			);

TOML	*_toml_clone_node(
			const TOML *const src
			);

int		_toml_fill_format(
			const char *const str,
			t_toml_str *const result,
			va_list *const args
			);

int		_toml_str_append_n(
			t_toml_str *const dest,
			const char *const src,
			const size_t len
			);

int		_toml_str_append_char(
			t_toml_str *const dest,
			const char c
			);

int		_toml_add_child(
			TOML *const parent,
			TOML *const child
			);

TOML	*_toml_find_child(
			TOML *const parent,
			const char *const key
			);

TOML	*_toml_get_field(
			TOML *const toml,
			const char *const field,
			const int depth
			);

void	*_toml_get(
			TOML *const obj,
			const char *const restrict key,
			va_list *const args
			);

size_t	_toml_node_size(
			const TOML *const node
			);

int		_toml_set_field(
			TOML **const toml,
			const char *const field,
			const char *const data,
			const int type
			);

int		_toml_set_node(
			TOML **const toml,
			const char *const field,
			const TOML *const value,
			const int type
			);

int		_toml_unset(
			TOML **const toml,
			const char *field,
			const int should_free,
			va_list *const args
			);

int		_toml_set_number_va_args(
			TOML *const toml,
			const char *const field,
			const char *const data,
			va_list *const args
			);

int		_toml_set_container_va(
			TOML *const toml,
			const char *const field,
			TOML *const var,
			const int type,
			va_list *const args
			);

int		_toml_set_wild(
			TOML *const toml,
			const char *const field,
			void *var,
			int type,
			va_list *const args
			);

int		_toml_set_string(
			TOML *const toml,
			const char *const field,
			char *var,
			va_list *const args
			);

int		_toml_array_append(
			TOML *const toml,
			void *const value,
			const int type
			);

int		_toml_array_pop(
			TOML *const toml,
			const size_t index
			);

int		_toml_set_from_array(
			TOML *toml,
			const char *const restrict field,
			void *const array,
			const size_t length,
			const int type,
			va_list *const args
			);

char	**_toml_split(
			const char *const field
			);

size_t	_toml_split_len(
			char **split
			);

int		_toml_count_field(
			const char *const field
			);

char	*_toml_itoa(
			long long value
			);

char	*_toml_uitoa(
			unsigned long long value
			);

int		_toml_parse_string(
			TOML **const toml,
			const char *const str,
			const char *const file
			);

TOML	*_toml_load_file(
			const char *const restrict path
			);

TOML	*_toml_load_str(
			const char *const restrict format,
			va_list *const args
			);

int		_toml_append_indent(
			t_toml_str *const out,
			const int depth,
			const int pretty
			);

int		_toml_append_value(
			t_toml_str *const out,
			TOML *const node,
			const int pretty
			);

int		_toml_append_table(
			t_toml_str *const out,
			TOML *const table,
			const char *const prefix,
			const int pretty
			);

char	*_toml_stringify(
			TOML *toml,
			const int pretty
			);

int		_toml_dump(
			TOML *const restrict toml,
			FILE *const file,
			const int pretty
			);

t_toml_error	*_toml_error(void);

__attribute__((destructor))
void			_toml_error_free(void);

int				_toml_error_set(
					const char *const	line_str,
					const char *const	file,
					const int			line,
					const int			col,
					const int			errnum
				);

const char		*_toml_error_str(
					const enum e_toml_errors	errnum
				);

char			*_toml_error_get(void);

void	_toml_perror(
	const char *const s
);

void	_toml_error_dump(
	FILE *const file
);

#endif	// CLIB_FILES_TOML_INTERNAL_H
