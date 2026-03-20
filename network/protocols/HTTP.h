/**
 * 
*/

#ifndef LIB_NET_PROTOCOLS_HTTP_H
# define LIB_NET_PROTOCOLS_HTTP_H

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "../../standards/lib_config.h"
# include "../../standards/lib_standards.h"

/* -----| Internals |----- */
# include "HTTP/types.h"
# include "HTTP/codes.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

# define	HTTP_VERSION(major, minor)	((major) * 100 + (minor))

# define	HTTP_1_0					HTTP_VERSION(1, 0)
# define	HTTP_1_1					HTTP_VERSION(1, 1)

# define	HTTP_MAX_SUPPORTED_VERSION	HTTP_1_0


# define	http_alloc(var) _Generic((var), \
	const char *const:	http_parse_str(var), \
	const int:			http_recv(var),       \
	default:			http_new()             \
)

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */
//--- Memory ---//
// - Alloc


t_http	*http_new(void);

/**
 * @brief	will choose automaticly the rigth version parsing based on the request header
 */
t_http	*http_parse_str(
	const char *s
);

t_http	*http_dup(
	const t_http *const first
);

t_http_header_list	*http_new_list(void);

t_http_header_list	*http_new_list_size_t(
	const size_t size
);

int	http_add_header(
	t_http_header_list *const list,
	t_http_header *const header
);

int	http_add_header_raw(
	t_http_header_list *const list,
	const char *const key,
	const char *const value
);

/**
 * if `buffer` == NULL --> allocate a new char *
 * else                --> write the raw request to the `buffer`
*/
char	*http_to_raw(
	const t_http *const http,
	char *const buffer,
	const size_t size
);

// - Free
void	http_free(
	t_http *const target
);

void				http_free_header_list(
	t_http_header_list *const list
);

void	http_free_header(
	t_http_header *const header
);

#endif	// LIB_NET_PROTOCOLS_HTTP_H