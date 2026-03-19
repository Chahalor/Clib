/**
 * 
 */

#ifndef LIB_NET_PROTOCOLS_HTTP_TYPES_H
# define LIB_NET_PROTOCOLS_HTTP_TYPES_H

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stddef.h>

/* -----| Globals   |----- */
	//...

/* -----| Internals |----- */
#include "../HTTP.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */
/* -----| Typename  |----- */

typedef float	t_http_version;

/* -----| Enums     |----- */

typedef enum e_http_methods	t_http_methods;

/* -----| Unions    |----- */
	//...

/* -----| Structs   |----- */

typedef struct s_http_header		t_http_header;
typedef struct s_http_header_list	t_http_header_list;
typedef struct s_http_body			t_http_body;
typedef struct s_http				t_http;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

/**
 * @enum	http_methods
*/
enum e_http_methods
{
	HTTP_ERROR = 0,

	HTTP_GET,
	HTTP_POST,
	HTTP_HEAD,

#if HTTP_MAX_SUPPORTED_VERSION >= HTTP_1_1
	HTTP_PUT,
	HTTP_DELETE,
	HTTP_OPTIONS,
	HTTP_TRACE,
	HTTP_CONNECT,
#endif

#if HTTP_MAX_SUPPORTED_VERSION >= HTTP_VERSION(1, 1)
	HTTP_PATCH,
#endif
};

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

struct s_http_header
{
	char	*key;
	char	*value;
};

struct s_http_header_list
{
	t_http_header	*data;	// probably an hash map
	int				size;
};

struct s_http_body
{
	char	*content;
	size_t	capacity;
};

struct s_http
{
	t_http_version		version;
	t_http_methods		method;
	t_http_header_list	headers;
	t_http_body			body;
	size_t				content_length;
};

#endif	// LIB_NET_PROTOCOLS_HTTP_TYPES_H