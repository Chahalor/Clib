/**
 * 
*/
#ifndef TYPES_INTERNAL_MAP_H
# define TYPES_INTERNAL_MAP_H
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
# include "../_types.h"

/* -----| Modules   |----- */
#include "processor/processor.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

# define	_POS(map, hash) ((size_t)((hash) % (map)->capacity))


/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

t_hash_map_entry	*_hash_map_entry_new(
	const char *const key
);

void	_hash_map_add_entry(
	t_hash_map			*const	map,
	t_hash_map_entry	*const	entry
);
void	_hash_map_free(
	t_hash_map *const	map,
	const char *const	key,
	const bool			should_free
);

t_hash_map_entry	*_entry_find(
	const t_hash_map *const	map,
	const char *const		key
);

t_hash_map_entry	*_entry_find_prev(
	t_hash_map *const	map,
	const char *const	key
);

int	_hash_map_add(
	t_hash_map *const	map,
	char *const			key,
	void *const			value
);

void	*_hash_map_find(
	t_hash_map *const	map,
	char *const			key
);

/**
 * @brief	return a pointer to the value at the key index
*/
void	**_hash_map_find_ptr(
	t_hash_map *const	map,
	char *const			key
);

void	_hash_map_remove(
	t_hash_map *const	map,
	char *const			key
);

bool	_hash_map_contains(
	const t_hash_map *const	map,
	const char *const		key
);

int	_hash_map_replace(
	t_hash_map *const	map,
	const char *const	key,
	void *const			value
);

/**
 * this function will have multiple known bucket size that reduce colition,
 * if `size` == -1, it will use the new bigger bucket size has the new size
*/
int	_hash_map_resize(
	t_hash_map *const	map,
	const ssize_t		size
);
#endif	// TYPES_INTERNAL_MAP_H
