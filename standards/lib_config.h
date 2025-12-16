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
	//...

/* -----| Internals |----- */
	//...

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE	1024
# endif //!BUFFER_SIZE

# define MEM_BUCKET_SIZE	1021

# define INTERFACE_JSON_STRING_ALLOC_SIZE	128

# ifndef MEM_MANAGER_TYPE
#  define MEM_MANAGER_TYPE	1
# endif //!MEM_MANAGER_TYPE

# define NET_MAX_LISTEN			16
# define NET_MAX_RETRY			128
# define NET_DEFAULT_NB_TRY	16

enum e_error_standard	// TODO: remove after checking it compile
{
	error_none = 0,
	error_invalid_arg,
	error_alloc_fail,
	error_invalid_file,
	error_not_implemented,
	error_syscall_fail,
	error_connection_fail
};
/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */
	//...

#endif	// !CONFIG_H