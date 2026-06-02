/**
 * 
*/
#ifndef TYPES_SETTING_H
# define TYPES_SETTING_H
# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stddef.h>
#include <sys/types.h>

/* -----| Globals   |----- */
# include "lib_config.h"
# include "lib_standards.h"

/* -----| Internals |----- */
# include "types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

# ifndef HASH_MAP_BUCKET_SIZE
#  define	HASH_MAP_BUCKET_SIZE	1021
# endif	// HASH_MAP_BUCKET_SIZE

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

typedef struct s_types_errors	t_types_errors;
typedef struct s_types_settings	t_types_settings;

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

struct s_types_errors
{
	int			errnum;
	const char	*key;
};

struct s_types_settings
{
	/* errors */
	t_types_errors	error;

	/* Memory */
	void			*(*alloc)(size_t);
	void			*(*realloc)(void *, size_t);
	void			(*free)(void *);
	void			*(*dup)(const void *, ssize_t);

	/* Implementation */
	size_t			(*hash_string)(const char *const);
};

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

extern t_types_settings	g_types_settings;

#endif	// TYPES_SETTING_H