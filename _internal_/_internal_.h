// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stdarg.h>

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
#pragma region Strings

/**
 * @brief	ensure _dest can append _add bytes (plus NUL)
 *
 * @param	_dest	target string buffer
 * @param	_add	number of bytes to append
 *
 * @return	error_none on success, or an error code
 */
int	_string_reserve_(
	_t_string_ *const restrict _dest,
	const size_t _add
);

/**
 * @brief	append a byte sequence to the json string
 *
 * @param	_dest	target string buffer
 * @param	_src	source bytes
 * @param	_len	number of bytes to append
 *
 * @return	error_none on success, or an error code
 */
int	_string_append_n_(
	_t_string_ *const restrict _dest,
	const char *const restrict _src,
	const size_t _len
);

/**
 * @brief	append a single character to the json string
 *
 * @param	_dest	target string buffer
 * @param	_c		character to append
 *
 * @return	error_none on success, or an error code
 */
int	_string_append_char_(
	_t_string_ *const restrict _dest,
	const char _c
);

/**
 * @brief	append an unsigned integer in base 2..16
 *
 * @param	_dest	target string buffer
 * @param	_value	unsigned value to append
 * @param	_base	numeric base (2..16)
 *
 * @return	error_none on success, or an error code
 */
int	_string_append_uint_base_(
	_t_string_ *const restrict _dest,
	unsigned long long _value,
	const unsigned int _base
);

/**
 * @brief	append a signed integer in base 10
 *
 * @param	_dest	target string buffer
 * @param	_value	signed value to append
 *
 * @return	error_none on success, or an error code
 */
int	_string_append_int_(
	_t_string_ *const restrict _dest,
	long long _value
);

/**
 * @brief	append a value based on the _json_format_type
 *
 * @param	_dest	target string buffer
 * @param	value	pointer to the value (or direct string for str)
 * @param	type	_json_format_type selector
 *
 * @return	error_none on success, or an error code
 */
int	_string_add_value_(
	_t_string_ *const restrict _dest,
	const void *const restrict value,
	const int type
);

/** */
__attribute__((visibility("hidden")))
_t_string_	*_string_new_(
	size_t length
);

/** */
__attribute__((visibility("hidden")))
_t_string_	*_string_new_form_char_(
	const char *const _src
);

/** */
__attribute__((visibility("hidden")))
int	_string_append_(
	_t_string_ *const target,
	const char *const src,
	const size_t length
);

/** */
__attribute__((visibility("hidden")))
void	_string_free_(
	_t_string_ *const target
);

/* -----|  Formats   |----- */
#pragma region Formats

/** */
int	_fill_format_(
	const char *const restrict _str,
	_t_string_ *const restrict _result,
	va_list *const restrict _args
);