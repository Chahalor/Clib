// Header

#ifndef CONFIG_H
# define CONFIG_H

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "config.h"
# include "standards.h"

/* -----| Internals |----- */
# include "types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE	1024
# endif //!BUFFER_SIZE

# define MEM_BUCKET_SIZE	1021

# define INTERFACE_JSON_STRING_ALLOC_SIZE	128//4096//128	//TODO: turn back to a lower alloc size (or not)

# ifndef MEM_MANAGER_TYPE
#  define MEM_MANAGER_TYPE	1
# endif //!MEM_MANAGER_TYPE

# define NET_MAX_LISTEN	16
# define NET_MAX_RETRY	128

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */
	//...

#endif	// !CONFIG_H