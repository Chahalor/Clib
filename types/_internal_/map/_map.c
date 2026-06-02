/**
 * 
*/
/* ----| Headers    |----- */
	/* Standard */
#include <string.h>

	/* Internal */
#include "_map.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

int	_hash_map_add(
	t_hash_map *const	map,
	char *const			key,
	void *const			value
)
{
	t_hash_map_entry	*new;

	new = _hash_map_entry_new(key);
	if (unlikely(!new))
		return (-1);

	new->value = value;
	_hash_map_add_entry(map, new);

	return (error_none);
}

void	*_hash_map_find(
	t_hash_map *const	map,
	char *const			key
)
{
	t_hash_map_entry *const	this = _entry_find(map, key);

	if (likely(this))
		return (this->value);

	return (NULL);
}

/**
 * @brief	return a pointer to the value at the key index
*/
void	**_hash_map_find_ptr(
	t_hash_map *const	map,
	char *const			key
)
{
	t_hash_map_entry *const	this = _entry_find(map, key);

	if (likely(this))
		return (&this->value);

	return (NULL);
}

void	_hash_map_remove(
	t_hash_map *const	map,
	char *const			key
)
{
	if (!_entry_find(map, key))
		return ;
	_hash_map_free(map, key, false);
	map->length--;
}

bool	_hash_map_contains(
	const t_hash_map *const	map,
	const char *const		key
)
{
	return (_entry_find(map, key) != NULL);
}

int	_hash_map_replace(
	t_hash_map *const	map,
	const char *const	key,
	void *const			value
)
{
	t_hash_map_entry *const	this = _entry_find(map, key);

	if (unlikely(!this))
		return (-1);

	this->value = value;
	return (error_none);
}

/**
 * this function will have multiple known bucket size that reduce colition,
 * if `size` == -1, it will use the new bigger bucket size has the new size
*/
int	_hash_map_resize(
	t_hash_map *const	map,
	const ssize_t		size
)
{
	t_hash_map_entry	**back = map->map;
	const size_t		old_capacity = map->capacity;
	const size_t		old_length = map->length;

	map->map = setting->alloc(sizeof(t_hash_map_entry *) * (size));
	if (unlikely(!map->map))
		return_error(-errno, NULL, -errno);

	memset(map->map, 0, sizeof(t_hash_map_entry *) * (size));
	map->capacity = size;
	map->length = 0;
	for (size_t i = 0; i < old_capacity; i++)
	{
		t_hash_map_entry	*this = back[i];

		if (!this)
			continue ;

		while (this)
		{
			t_hash_map_entry *const	next = this->next;

			this->next = NULL;
			_hash_map_add_entry(map, this);
			this = next;
		}
	}
	setting->free(back);
	if (map->length != old_length)
		return (-1);

	return (error_none);
}
