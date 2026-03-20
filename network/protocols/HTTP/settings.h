/**
 * 
*/

#ifndef LIB_NET_PROTOCOLS_HTTP_SETTINGS_H
# define LIB_NET_PROTOCOLS_HTTP_SETTINGS_H

# pragma once

// Header

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
/*                                 Enums                                      */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

typedef struct s_http_settings
{
	void	*allocator;	// showed has `void *` but this is an internal struct
	int		errno;
	int		version_min;
	int		version_max;
}	t_http_settings;

extern t_http_settings	g_net_prot_http_settings;

#endif	// LIB_NET_PROTOCOLS_HTTP_SETTINGS_H