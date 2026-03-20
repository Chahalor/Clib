/**
 * 
*/

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "lib_config.h"
# include "lib_standards.h"

/* -----| Internals |----- */
# include "_types.h"
# include "../settings.h"
# include "../error.h"

/* -----| Modules   |----- */
# include "../../../../memory.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

extern struct _s_http_allocators	_g_net_prot_http_allocator;

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

t_http	*_http_new(
	const float version,
	const t_http_methods method,
	t_http_header_list *const headers,
	t_http_body *const body
);

t_http_header	*_http_new_header(
	const char *const key,
	const char *const value
);

/**
 * if `all` == 1 also free the list of header
*/
void	_http_free(
	t_http *const target,
	const int all
);

/**
 * if `recursive` == 1, free all element of the list
*/
void	_http_free_list(
	t_http_header_list *const list,
	const int recursive
);