// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stdarg.h>
#include <stdint.h>

/* -----| Globals   |----- */
// # include "lib_config.h"
// # include "lib_standards.h"

/* -----| Internals |----- */
# include "../_json.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/* -----| Logic |----- */

/**
 * @brief	cleannly add a new value to the child chain of the node
 */
int	_json_add_child(
		t_json **_target,
		t_json *_child
		);

/**
 * @brief	cleannly add a new value to the next chain of the node
 */
int	_json_add_next(
		t_json **_target,
		t_json *_next
		);

/* -----| Unset |----- */

/**
 * @brief	unset the value of field.
 * 
 * will free recursivly if `free == true`.
 * 
 * will fill the va_args in `field` if `args != NULL`
 */
int	_json_unset(
		JSON **_json,
		const char *restrict _field,
		const int _free,
		va_list *const restrict _args
		);

/* -----| Set   |----- */

int	_json_set_int32(
	JSON *const json,
	const char *const field,
	int32_t var, 
	va_list *const restrict _args
);

int	_json_set_uint32(
	JSON *const json,
	const char *const field,
	uint32_t var,
	va_list *const restrict _args
);

int	_json_set_int64(
	JSON *const json,
	const char *const field,
	int64_t var,
	va_list *const restrict _args
);

int	_json_set_uint64(
	JSON *const json,
	const char *const field,
	uint64_t var,
	va_list *const restrict _args
);

int	_json_set_array(
	JSON **_json,
	const char *const restrict _field,
	const void *_value
);

int	_json_set_obj(
	JSON **_json,
	const char *const restrict _field,
	const void *_value
);

int	_json_set_str(
	JSON **_json,
	const char *const restrict _field,
	const void *_value
);

int	_json_set_bool(
	JSON **_json,
	const char *const restrict _field,
	const void *_value
);

int	_json_set_null(
	JSON **_json,
	const char *const restrict _field
);

/**
 * @brief	set the value of `field` to `data`.

 */
int	_json_set_field(
		t_json **_json,
		const char *const restrict _field,
		const char *const restrict _data,
		const int _type
		);

/**
 * @brief	set the value of the node `field` to `value`.
 */
int	_json_set(
		JSON **_json,
		const char *const restrict _field,
		const void *_value,
		const int _type
		);

/**
 * @brief	set the value of the node `field` to `value`.
 * 
 * handle va args for `field` and for `value` if `type == json_tok_str`
 */
int	_json_set_va_args(
		JSON **_json,
		const char *const restrict _field,
		const void *_value,
		const unsigned int _type,
		va_list *const restrict _args
		);

/**
 * @brief	set the array value of the node `field` to `array`.
 * 
 * handle va args for `field`
 */
int	_json_set_array_va_list(
		JSON **_json,
		const char *const restrict _field,
		void *const _array,
		const size_t _length,
		const int _type,
		va_list *const restrict _args
		);

/**
 * @brief	append a new value to the end of the array
*/
int	_json_append_array(
		JSON *_json,
		void *const _value,
		const int _type
		);

/** */
int	_json_pop_array(
	JSON *const _json,
	const size_t _index
);

/**
 * @brief	set the array value of the node `field` to `array`.
 * 
 * handle va args for `field`
 */
int	_json_set_array_va_args(
		JSON *_json,
		const char *const restrict _field,
		void *const _array,
		const size_t _length,
		const int _type,
		...
		);
