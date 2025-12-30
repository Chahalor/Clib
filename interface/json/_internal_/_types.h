// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stddef.h>
#include <stdio.h>

/* -----| Globals   |----- */
	//...

/* -----| Internals |----- */
#include "../json.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */

typedef struct s_json_str		t_json_str;
typedef struct s_json_parser	t_json_parser;
typedef struct s_json			t_json;
typedef t_json					JSON;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

/**
 * @enum	_json_format_type
 * 
 * @brief	all handled type to add for format functions
 * 
 * @version	1.0.0
 */
enum e_json_format_type
{
	_json_format_type_char,
	_json_format_type_str,
	_json_format_type_int,
	_json_format_type_long,
	_json_format_type_hex,
};

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

struct s_json_str
{
	char	*content;	// string content
	int		len;		// len of the string
	int		size;		// alloc size
};

struct s_json_parser
{
	char	*buf;
	size_t	len;
	size_t	i;
};

struct s_json
{
	char	*key;		// NULL if element of an array
	int		type;		// type of the json node (str, int, bool, etc, ...)

	char	*data;

	t_json	*child;		// first child (objet or array)
	t_json	*next;		// next element of the same parent
};
