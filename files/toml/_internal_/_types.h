/**
 * @file _types.h
 * @brief Internal TOML tree declarations.
*/
#ifndef CLIB_FILES_TOML_INTERNAL_TYPES_H
# define CLIB_FILES_TOML_INTERNAL_TYPES_H

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stddef.h>

/* -----| Globals   |----- */
	//...

/* -----| Internals |----- */
#include "../types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */
/* -----| Typename  |----- */
	//...

/* -----| Enums     |----- */
	//...

/* -----| Unions    |----- */
	//...

/* -----| Structs   |----- */

typedef struct s_toml_str	t_toml_str;
typedef struct s_toml		t_toml;
typedef struct s_toml_error	t_toml_error;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

struct s_toml_str
{
	char	*content;
	size_t	len;
	size_t	size;
};

struct s_toml
{
	char	*key;
	char	*data;
	t_toml	*child;
	t_toml	*next;
	int		type;
};

struct s_toml_error
{
	char				*file;
	char				*str_line;
	int					line;
	int					col;
	enum e_toml_errors	errnum;	// errnum > 0 use the toml error enum and if errnum < 0, use the errno defines
};

#endif	// CLIB_FILES_TOML_INTERNAL_TYPES_H
