/**
 * @file types.h
 * @brief Public TOML module types.
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
	//...

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

typedef enum e_toml_type	t_toml_type;

/* -----| Unions    |----- */
	//...

/* -----| Structs   |----- */

typedef struct s_toml	TOML;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

enum e_toml_type
{
	toml_tok_null = 1 << 0,
	toml_tok_bool = 1 << 1,
	toml_tok_int = 1 << 2,
	toml_tok_float = 1 << 3,
	toml_tok_str = 1 << 4,
	toml_tok_array = 1 << 5,
	toml_tok_table = 1 << 6,
	toml_tok_datetime = 1 << 7,
	TOML_NULL = toml_tok_null,
	TOML_BOOL = toml_tok_bool,
	TOML_INT = toml_tok_int,
	TOML_FLOAT = toml_tok_float,
	TOML_STRING = toml_tok_str,
	TOML_ARRAY = toml_tok_array,
	TOML_TABLE = toml_tok_table,
	TOML_DATETIME = toml_tok_datetime,
	toml_null = toml_tok_null,
	toml_bool = toml_tok_bool,
	toml_int = toml_tok_int,
	toml_float = toml_tok_float,
	toml_str = toml_tok_str,
	toml_array = toml_tok_array,
	toml_table = toml_tok_table,
	toml_datetime = toml_tok_datetime
};

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */


#endif	// CLIB_FILES_TOML_TYPES_H
