/**
 * 
*/
/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "types.h"
#include "_internal_/map/_map.h"

	/* External */
#include "../memory.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

t_types_settings	g_types_settings = {
	.error = {0},
	.alloc = mem_alloc,
	.realloc = mem_realloc,
	.free = mem_free,
	.dup = mem_dup,
	.hash_string = NULL	// TODO: add an string hashing function
};

int	hash_map_add(
	t_hash_map *const	map,
	char *const			key,
	void *const			value
)
{
	if (unlikely(!map || !key))
		return_error(error_invalid_arg, key, error_invalid_arg);

	return (_hash_map_add(map, key, value));
}

void	*hash_map_find(
	t_hash_map *const	map,
	char *const			key
)
{
	if (unlikely(!map || !key))
		return_error(error_invalid_arg, key, NULL);

	return (_hash_map_find(map, key));
}

/**
 * @brief	return a pointer to the value at the key index
*/
void	**hash_map_find_ptr(
	t_hash_map *const	map,
	char *const			key
)
{
	if (unlikely(!map || !key))
		return_error(error_invalid_arg, key, NULL);

	return (_hash_map_find_ptr(map, key));
}

void	hash_map_remove(
	t_hash_map *const	map,
	char *const			key
)
{
	if (unlikely(!map || !key))
	{
		setting->error.errnum = error_invalid_arg;
		setting->error.key = key;
		return ;
	}

	return (_hash_map_remove(map, key));
}

bool	hash_map_contains(
	const t_hash_map *const	map,
	const char *const		key
)
{
	if (unlikely(!map || !key))
		return_error(error_invalid_arg, key, false);

	return (_hash_map_contains(map, key));
}

int	hash_map_replace(
	t_hash_map *const	map,
	const char *const	key,
	void *const			value
)
{
	if (unlikely(!map || !key))
		return_error(error_invalid_arg, key, error_invalid_arg);

	return (_hash_map_replace(map, key, value));
}

/**
 * this function will have multiple known bucket size that reduce colition,
 * if `size` == -1, it will use the new bigger bucket size has the new size
*/
int	hash_map_resize(
	t_hash_map *const	map,
	const ssize_t		size
)
{
	if (unlikely(!map || !size))
		return_error(error_invalid_arg, NULL, error_invalid_arg);

	return (_hash_map_resize(map, size));
}
