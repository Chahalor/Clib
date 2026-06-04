/**
 * 
*/
#ifndef CLIB_TYPE_H
# define CLIB_TYPE_H
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
# include "types/types.h"

/* -----| Modules   |----- */
# include "processor/processor.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

// #define	ARRAY_ACCESS(type, array, index)									\
// 	__builtin_choose_expr(													\
// 		IS_TYPE(typeof(array), t_array),									\
// 		(*(type *)((char *)(array).data + (array).elt_size * (index))),		\
// 		(*(type *)((char *)(array)->data + (array)->elt_size * (index)))	\
// 	)

#define hash_map_foreach (var, map) \
for (uint __i = 0; __i < HASH_MAP_BUCKET_SIZE; __i++)	\
// {														\
// 	struct s_hash_map_entry	*__this = &map->map[__i];	\
// 	if (!__this->)
// }

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

#pragma region Array
// int	array_alloc(
// 	t_array *const	array,
// 	const size_t	capacity
// );

// void	array_free(
// 	t_array *const	array
// );

// int		array_append(
// 	t_array *const	array,
// 	void *const		data
// );

// void	array_pop(
// 	t_array *const	array,
// 	const bool		free
// );

#pragma region Hash Map

size_t	hash_fnv1a(
	const char *str
);

int	hash_map_setup(
	t_hash_map *const	map,
	const size_t		size
);

int	hash_map_destroy(
	t_hash_map *const map
);

int	hash_map_add(
	t_hash_map *const	map,
	char *const			key,
	void *const			value
);

void	*hash_map_find(
	t_hash_map *const	map,
	char *const			key
);

/**
 * @brief	return a pointer to the value at the key index
*/
void	**hash_map_find_ptr(
	t_hash_map *const	map,
	char *const			key
);

void	hash_map_remove(
	t_hash_map *const	map,
	char *const			key
);

bool	hash_map_contains(
	const t_hash_map *const	map,
	const char *const		key
);

int hash_map_replace(
	t_hash_map *const	map,
	const char *const	key,
	void *const			value
);

/**
 * this function will have multiple known bucket size that reduce colition,
 * if `size` == -1, it will use the new bigger bucket size has the new size
*/
int	hash_map_resize(
	t_hash_map *const	map,
	const ssize_t		size
);

#endif	// CLIB_TYPE_H