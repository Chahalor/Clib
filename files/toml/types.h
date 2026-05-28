/**
 * @file types.h
 * @brief Public types used by the TOML module.
 *
 * This header exposes the type identifiers accepted by the TOML public API and
 * the public handle type used to address TOML documents and nodes. The concrete
 * node layout is intentionally kept outside this file; callers should prefer
 * the functions in `files/toml.h` for allocation, lookup, mutation, and output.
*/
#ifndef CLIB_FILES_TOML_TYPES_H
# define CLIB_FILES_TOML_TYPES_H

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "../../standards/lib_config.h"

/* -----| Internals |----- */
	//...

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */
/* -----| Typename  |----- */
	//...

/* -----| Enums     |----- */

/**
 * @typedef t_toml_type
 * @brief TOML value kind used by setters, appenders, and type checks.
 *
 * Values of this enum describe how a node's stored data should be interpreted.
 * The enum is also exposed through uppercase and lowercase aliases so callers
 * can use the style that best matches their codebase.
 *
 * @scope Public API. Pass these values to functions such as `toml_set_wild()`,
 * `toml_array_append()`, `toml_set_from_array()`, and `toml_assert_type()`.
 */
typedef enum e_toml_type	t_toml_type;

/* -----| Unions    |----- */
	//...

/* -----| Structs   |----- */

/**
 * @typedef TOML
 * @brief Public handle for a TOML document, table, array, or scalar node.
 *
 * A `TOML *` returned by `toml_new()`, `toml_load_file()`, or
 * `toml_load_str()` owns a tree of nodes and must be released with
 * `toml_unload()` or `toml_free()`. Lookup functions can also return `TOML *`
 * handles for table and array nodes; those child handles are borrowed and must
 * not be freed separately from their root document.
 *
 * @scope Public API handle. The concrete fields are implementation details
 * except where legacy helper macros such as `toml_foreach()` expose traversal.
 */
typedef struct s_toml	TOML;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

/**
 * @enum e_toml_type
 * @brief TOML value categories understood by the module.
 *
 * Scalar values are stored as strings internally and converted to TOML text
 * during stringify or dump. Container values (`toml_tok_array` and
 * `toml_tok_table`) own child nodes.
 *
 * @example
 * @code
 * TOML *doc = toml_new();
 * toml_set_wild(doc, "enabled", &(int){1}, toml_bool);
 * toml_set_wild(doc, "name", "clib", toml_str);
 * toml_assert_type((TOML *)toml_get(doc, "items"), toml_array);
 * @endcode
 */
enum e_toml_type
{
	/** Empty or explicit null-like value. */
	toml_tok_null = 1 << 0,
	/** Boolean value, written as `true` or `false`. */
	toml_tok_bool = 1 << 1,
	/** Signed or unsigned integer value stored in decimal text form. */
	toml_tok_int = 1 << 2,
	/** Floating-point value stored as caller-provided text. */
	toml_tok_float = 1 << 3,
	/** Quoted string value. */
	toml_tok_str = 1 << 4,
	/** Ordered array container. */
	toml_tok_array = 1 << 5,
	/** Key/value table container. */
	toml_tok_table = 1 << 6,
	/** Date/time value stored as caller-provided TOML-compatible text. */
	toml_tok_datetime = 1 << 7,
	/** Uppercase alias for `toml_tok_null`. */
	TOML_NULL = toml_tok_null,
	/** Uppercase alias for `toml_tok_bool`. */
	TOML_BOOL = toml_tok_bool,
	/** Uppercase alias for `toml_tok_int`. */
	TOML_INT = toml_tok_int,
	/** Uppercase alias for `toml_tok_float`. */
	TOML_FLOAT = toml_tok_float,
	/** Uppercase alias for `toml_tok_str`. */
	TOML_STRING = toml_tok_str,
	/** Uppercase alias for `toml_tok_array`. */
	TOML_ARRAY = toml_tok_array,
	/** Uppercase alias for `toml_tok_table`. */
	TOML_TABLE = toml_tok_table,
	/** Uppercase alias for `toml_tok_datetime`. */
	TOML_DATETIME = toml_tok_datetime,
	/** Short alias for `toml_tok_null`. */
	toml_null = toml_tok_null,
	/** Short alias for `toml_tok_bool`. */
	toml_bool = toml_tok_bool,
	/** Short alias for `toml_tok_int`. */
	toml_int = toml_tok_int,
	/** Short alias for `toml_tok_float`. */
	toml_float = toml_tok_float,
	/** Short alias for `toml_tok_str`. */
	toml_str = toml_tok_str,
	/** Short alias for `toml_tok_array`. */
	toml_array = toml_tok_array,
	/** Short alias for `toml_tok_table`. */
	toml_table = toml_tok_table,
	/** Short alias for `toml_tok_datetime`. */
	toml_datetime = toml_tok_datetime
};

enum e_toml_errors
{
	/** No error. */
	TOML_ERROR_NONE = error_none,
	/** A public or internal API received an invalid argument. */
	TOML_ERROR_INVALID_ARG = error_invalid_arg,
	/** Memory allocation failed. */
	TOML_ERROR_ALLOC_FAIL = error_alloc_fail,
	/** A file handle or file path is invalid. */
	TOML_ERROR_INVALID_FILE = error_invalid_file,
	/** Requested TOML behavior is not implemented. */
	TOML_ERROR_NOT_IMPLEMENTED = error_not_implemented,
	/** A system or I/O call failed. */
	TOML_ERROR_SYSCALL_FAIL = error_syscall_fail,
	/** Generic connection failure alias from the shared error set. */
	TOML_ERROR_CONNECTION_FAIL = error_connection_fail,
	/** Generic TOML parsing failure. */
	TOML_ERROR_PARSE,
	/** The parser reached the end of input while still expecting content. */
	TOML_ERROR_UNEXPECTED_EOF,
	/** The parser found a token that is not valid at the current position. */
	TOML_ERROR_UNEXPECTED_TOKEN,
	/** A key is malformed. */
	TOML_ERROR_INVALID_KEY,
	/** A key is empty. */
	TOML_ERROR_EMPTY_KEY,
	/** A dotted key or path is malformed. */
	TOML_ERROR_INVALID_PATH,
	/** A dotted path conflicts with an existing scalar value. */
	TOML_ERROR_PATH_CONFLICT,
	/** A table declaration is malformed. */
	TOML_ERROR_INVALID_TABLE,
	/** A table was declared more than once. */
	TOML_ERROR_DUPLICATE_TABLE,
	/** A key was assigned more than once. */
	TOML_ERROR_DUPLICATE_KEY,
	/** A value is malformed or unsupported. */
	TOML_ERROR_INVALID_VALUE,
	/** A string value is malformed. */
	TOML_ERROR_INVALID_STRING,
	/** A quoted string was not terminated. */
	TOML_ERROR_UNTERMINATED_STRING,
	/** A string escape sequence is invalid. */
	TOML_ERROR_INVALID_ESCAPE,
	/** An integer value is malformed. */
	TOML_ERROR_INVALID_INTEGER,
	/** A floating-point value is malformed. */
	TOML_ERROR_INVALID_FLOAT,
	/** A boolean value is malformed. */
	TOML_ERROR_INVALID_BOOL,
	/** A date/time value is malformed. */
	TOML_ERROR_INVALID_DATETIME,
	/** An array value is malformed. */
	TOML_ERROR_INVALID_ARRAY,
	/** An array value was not terminated. */
	TOML_ERROR_UNTERMINATED_ARRAY,
	/** An inline table value is malformed. */
	TOML_ERROR_INVALID_INLINE_TABLE,
	/** An inline table value was not terminated. */
	TOML_ERROR_UNTERMINATED_INLINE_TABLE,
	/** A TOML value type is invalid for the requested operation. */
	TOML_ERROR_INVALID_TYPE,
	/** A source TOML node type does not match the requested destination type. */
	TOML_ERROR_TYPE_MISMATCH,
	/** An array index is outside the array bounds. */
	TOML_ERROR_INDEX_OUT_OF_RANGE,
	/** Number of TOML error values. */
	TOML_ERROR_COUNT
};

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */


#endif	// CLIB_FILES_TOML_TYPES_H
