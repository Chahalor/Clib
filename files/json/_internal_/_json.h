// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "../../../standards/lib_config.h"
# include "../../../standards/lib_standards.h"

/* -----| Internals |----- */
# include "_types.h"
# include "../../json.h"
# include "edit/_json_edit.h"

/* -----| Modules   |----- */
#include "../../../lib.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

#define _JSON_FOEACH_ARRAY(var, array_node)													\
	for (t_json *(var) = ((t_json *)array_node ? ((t_json *)(array_node))->child : NULL);	\
		(var) != NULL;																		\
			(var) = (var)->next)


/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/* -----| memory |----- */
#pragma region Memory

JSON	*_json_new_root(
			const char *const restrict _filename
			);

t_json	*_json_new_content(
			const char *_key,
			int _type,
			char *_data
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

/**
 * @brief	clone the current node by duplicating it recursivly
 */
t_json	*_json_clone_node(
			const t_json *_src
			);

/* -----| format |----- */
#pragma region Format
/**
 * @brief	va remplir le string result en se basant sur le format _str
 *
 * @param	_str	format string
 * @param	_result	output json string buffer
 * @param	_args	va_list with format values
 *
 * @return	error_none on success, or an error code
 */
	int		_json_fill_format(
				const char *const restrict _str,
				t_json_str *const restrict _result,
				va_list *const restrict _args
				);

	/* -----| string |----- */

	int		_json_str_reserve(
				t_json_str *const restrict _dest,
				const size_t _add
				);

	int		_json_str_append_n(
				t_json_str *const restrict _dest,
				const char *const restrict _src,
				const size_t _len
				);

	int		_json_str_append_char(
				t_json_str *const restrict _dest,
				const char _c
				);

	int		_json_append_uint_base(
				t_json_str *const restrict _dest,
				unsigned long long _value,
				const unsigned int _base
				);

	int		_json_append_int(
				t_json_str *const restrict _dest,
				long long _value
				);

	int		_json_add_value(
				t_json_str *const restrict _dest,
				const void *const restrict value,
				const int type
				);

	/**
	 * @brief	allocate a `t_json_str` of `size` len
	 * 
	 * add the null bit at the end of content
	 * 
	 */
	t_json_str	*_json_new_str(
					const int _size
					);

/* -----| get    |----- */
#pragma region GET

/** */
int		_json_get(
			JSON *const restrict _json,
			const char *const restrict _format,
			va_list *const restrict _args,
			void *restrict *const restrict _result
			);

/* -----| parser |----- */
#pragma region Parser

/**
 * @brief	parse and load a JSON variable based on the given format
 * 
 * @param	json	pointer to the JSON to fill
 * @param	str		the format to load
 * @param	args	the va list containing all args for the format
 * 
 * @return	err_none on succes, or the error code
 */
int		_json_parser_va_string(
			JSON **_json,
			const char *const restrict _str,
			va_list *const restrict _args
			);

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
#pragma region Access

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

size_t	_json_node_size(
			const t_json *_node
			);

size_t	_json_access_len(
			const t_json *_json
			);

/* -----| Saving |----- */
#pragma region Saving

int		_json_dump(
			JSON *const restrict _json,
			FILE *const _file,
			const int pretty
			);

char	*_json_stringify(
			JSON *_json,
			const int pretty,
			int *const restrict errnum
			);

/* -----| tools  |----- */
#pragma region Tools

static inline
int		_json_tool_is_sep(
	const char _c
)
{
	return (_c == '.' || _c == '/');
}

static inline
int		_json_is_type_complex(
	const int _type
)
{
	return (_type & json_tok_array || _type & json_tok_obj);
}

static inline
int		_json_is_type_nbr(
	const int _type
)
{
	return (_type & json_tok_nbr);
}

static inline
int		_json_is_type_str(
	const int _type
)
{
	return (_type & json_tok_str);
}

static inline
int		_json_is_type_bool(
	const int _type
)
{
	return (_type & json_tok_bool);
}

static inline
int		_json_is_type_null(
	const int _type
)
{
	return (_type & json_tok_null);
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
#pragma region Debug

void	print_json_tree(
			t_json *node,
			int depth
			);
