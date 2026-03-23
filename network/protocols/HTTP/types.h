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
#include <stdint.h>

/* -----| Globals   |----- */
	//...

/* -----| Internals |----- */
// #include "../HTTP.h"

/* -----| Modules   |----- */
	//...

# ifndef HTTP_VERSION
#  define	HTTP_VERSION(major, minor)	((major) * 100 + (minor))
# endif	//HTTP_VERSION

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */
/* -----| Typename  |----- */

typedef uint16_t	t_http_version;

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
	char			*key;
	char			*value;

	t_http_header	*next;			// hash-map linked list
	t_http_header	*order_prev;	// prev in insert order
	t_http_header	*order_next;	// next in insert order
};

struct s_http_header_list
{
	t_http_header	**data;		// an hash map
	t_http_header	*head;
	t_http_header	*tail;

	int				capacity;	// size of the has map
	int				size;		// number of headers
};

struct s_http_body
{
	char	*content;
	size_t	capacity;
};

struct s_http
{
	t_http_version		version;	// 100 = 1.0, 101 = 1.1
	t_http_methods		method;
	t_http_header_list	headers;
	t_http_body			body;
	char				*path;
	size_t				content_length;
};

#endif	// LIB_NET_PROTOCOLS_HTTP_TYPES_H