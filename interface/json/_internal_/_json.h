// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "config.h"
# include "standards.h"

/* -----| Internals |----- */
# include "_types.h"
# include "../json.h"

/* -----| Modules   |----- */
#include "lib.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

#define _JSON_FOEACH_ARRAY(_var, _node) _var = ((JSON *)_node)->child; \
for (0; _var != NULL; _var = _var->next)

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/* -----| memory |----- */

JSON	*_json_new_root(
			const char *const restrict _filename
			);

t_json	*_json_new_content(
			const char *_key,
			int _type,
			char *_data
			);

int		_json_free_root(
			JSON *_json
			);

void	_json_free_content(
			t_json *_content
			);

int		_json_remove_node(
			t_json **_head,
			t_json *_target
			);

int		_json_free_all(
			JSON *_target
			);

/* -----| parser |----- */

int		_json_parser_string(
			JSON **_json,
			char *_str,
			const size_t _len
			);

int		_json_parser_file(
			JSON **_json,
			const char *const restrict _filename
			);

/* -----| access |----- */

t_json	*_json_get_field(
			t_json *_json,
			const char *const restrict _field,
			const int _depth
			);

void	*_json_access_field(
			t_json *_json,
			const char *const restrict _field,
			const int _depth
			);


/* -----| set    |----- */

int		_json_add_child(
			t_json **_target,
			t_json *_child
			);


int		_json_set_field(
			t_json **_json,
			const char *const restrict _field,
			const char *const restrict _data,
			const int _type
			);

int		_json_set(
			JSON **_json,
			const char *const restrict _field,
			const void *_value,
			const int _type
			);

int		_json_unset(
			JSON **_json,
			const char *const restrict _field
			);

/* -----| Saving |----- */

char	*_json_stringify(
			JSON *_json,
			const int pretty
			);

int		_json_dump(
			JSON *_json,
			FILE *_file,
			const int pretty
			);

/* -----| tools  |----- */

static inline
int		_json_tool_is_sep(
	const char _c
)
{
	return (_c == '.' || _c == '/');
}

int		_json_tool_count_field(
			const char *const restrict _str
			);

char	*_json_tool_itoa(
			long long _value
			);

char	**_json_tool_split(
			const char *const restrict _field
			);

size_t	_json_tool_array_len(
			const void *const _array,
			const int _elt_size
			);

/* -----| debug |----- */

void	print_json_tree(
			t_json *node,
			int depth
			);