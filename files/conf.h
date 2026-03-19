/**
 * 
*/

#ifndef LIB_FILES_CONF_H
# define LIB_FILES_CONF_H

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
# include "conf/types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

//--- Memory ---//
// - alloc

/**
 * @brief	allocate an empty conf struct
*/
CONF	*conf_new(void);

/**
 * @brief	load a conf from a file
*/
CONF	*conf_load(const char *path);

/**
 * @brief	load a conf from a `char *`
*/
CONF	*conf_load_str(const char *str);

// - free
/**
 * @brief	free the conf struct
*/
void	conf_free(CONF *const conf);

//--- Access ---//

/**
 * @brief	get the node at the key
 * 
 * @example	
 * ```C
 * char	*name = conf_get(main_conf, "server_name");
 * CONF	*location = conf_get(main_conf, "location");
 * CONF	*root = conf_get(location, "/");
 * char	**methods = conf_get(root, "methods");
 * 
 * should give the same access
 * 
 * char	**methods = conf_get(main_conf, "location./.methods")
*/
void	*conf_get(const CONF *const conf, const char *const key);

//--- Management ---//
// - Add

/**
 * @brief	create/update the value at the key `key` with the int `value`
*/
int	conf_add_int(CONF *const conf, const char *const key, const int value);

/**
 * @brief	create/update the value at the key `key` with the str `value`
*/
int	conf_add_str(CONF *const conf, const char *const key, const char *const value);

/**
 * @brief	create/update the value at the key `key` with the array of str `value`
*/
int	conf_add_array(CONF *const conf, const char *const key, const char *const *const value);

/**
 * @brief	create/update the value at the key `key` with the node `value`
*/
int	conf_add_node(CONF *const conf, const char *const key, CONF *const value);

// - Remove

/**
 * 
*/
int	conf_remove(CONF *const conf, const char *const key);

// - Check

/**
 * @brief	return true or false of the `conf` node type is equal to `type`
*/
inline int	conf_assert_type(const CONF *const conf, const t_conf_node_type type);

/**
 * @brief	return the node type of `conf`
*/
inline t_conf_node_type	conf_get_type(const CONF *const conf);

#endif	// LIB_FILES_CONF_H