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
			const char *const str
			);

TOML	*_toml_load_file(
			const char *const restrict path
			);

TOML	*_toml_load_str(
			const char *const restrict format,
			va_list *const args
			);
