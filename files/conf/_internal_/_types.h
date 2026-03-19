/**
 * 
*/

#ifndef _LIB_CONF__TYPE_H
# define _LIB_CONF__TYPE_H

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
# include <stddef.h>

/* -----| Globals   |----- */
	//...

/* -----| Internals |----- */
# include "../types.h"
# include "../setting.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */
/* -----| Typename  |----- */
	//...

/* -----| Enums     |----- */
	//...

/* -----| Unions    |----- */

typedef union _u_conf_node			_t_conf_node_content;	//

/* -----| Structs   |----- */
typedef struct _s_conf_allocator	_t_conf_allocator;		//
typedef struct _s_conf_value		_t_conf_value;			//
typedef struct _s_conf_node			_t_conf_node;			//

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */

/**
 * @union _conf_node
*/
union _u_conf_node
{
	int						nb;
	char					*str;
	char					**array;
	_t_conf_node_content	*node;
};

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

/**
 * @struct _conf_allocator
*/
struct _s_conf_allocator
{
	t_alloc_fn		alloc;
	t_realloc_fn	realloc;
	t_free_fn		free;
};

struct _s_conf_value
{
	_t_conf_node_content	data;
	t_conf_node_type		type;
};

/**
 * @struct _conf_node
*/
struct _s_conf_node
{
	_t_conf_value	value;
	char			*key;

	_t_conf_node	*child;
	_t_conf_node	*next;
};

#endif	// _LIB_CONF__TYPE_H