/**
 * 
*/

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <ctype.h>

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

#pragma region Memory

static inline void	_lower(
	char *str
)
{
	while (*str)
	{
		*str = tolower(*str);
		str++;
	}
}

t_http	*_http_new(
	const float version,
	const t_http_methods method,
	t_http_header_list headers,
	t_http_body body
);

t_http_header	*_http_new_header(
	const char *const key,
	const char *const value
);

int	_http_setup_header_list(
	t_http_header_list *const list,
	const int capacity
);

int	_http_add_header(
	t_http_header_list *const list,
	t_http_header *const header
);

void	_http_remove_header(
	t_http_header_list *const list,
	const char *const key,
	const int free
);

t_http_header	*_http_header_find(
	t_http_header_list *const list,
	const char *const key
);

/**
 * if `all` == 1 also free the list of header
*/
void	_http_free(
	t_http *const target,
	const int all
);

void	_http_free_header(
	t_http_header *const header,
	const int recursive
);

/**
 * if `recursive` == 1, free all element of the list
*/
void	_http_free_list(
	t_http_header_list *const list,
	const int recursive
);

#pragma region Methods

const char	*_http_methods_str(
	const t_http_methods method
);

t_http_methods	_http_str_to_method(
	const char *str
);