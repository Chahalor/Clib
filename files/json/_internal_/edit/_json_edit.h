// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

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

/** */
int	_json_append_array(
		JSON *_json,
		void *const _value,
		const int _type
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
