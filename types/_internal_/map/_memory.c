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

t_hash_map_entry	*_hash_map_entry_new(
	const char *const key
)
{
	t_hash_map_entry	*result;

	result = setting->alloc(sizeof(*result) + sizeof(char) * (strlen(key) + 1));
	if (unlikely(!result))
	{
		setting->error.errnum = -errno;
		setting->error.key = key;
		return (NULL);
	}
	result->key = (char *)(result + 1);
	strcpy(result->key, key);
	result->value = NULL;
	result->next = NULL;
	result->hash = setting->hash_string(key);

	return (result);
}

void	_hash_map_add_entry(
	t_hash_map			*const	map,
	t_hash_map_entry	*const	entry
)
{
	const size_t		hash = setting->hash_string(entry->key);
	const size_t		pos = _POS(map, hash);
	t_hash_map_entry	*this = map->map[pos];

	entry->hash = hash;
	entry->next = NULL;
	while (this && this->next)
		this = this->next;

	if (!this)
		map->map[pos] = entry;
	else
		this->next = entry;

	map->length++;
}

void	_hash_map_free(
	t_hash_map *const	map,
	const char *const	key,
	const bool			should_free
)
{
	t_hash_map_entry	*this = _entry_find(map, key);
	t_hash_map_entry	*prev = _entry_find_prev(map, key);

	if (unlikely(!this))
		return ;

	if (prev)
		prev->next = this->next;
	else
		map->map[_POS(map, this->hash)] = this->next;

	if (should_free)
		setting->free(this->value);

	setting->free(this);
}

int	_hash_map_setup(
	t_hash_map *const map
)
{
	if (map->map)
		setting->free(map->map);

	map->map = setting->alloc(sizeof(t_hash_map_entry *) * map->capacity);
	if (unlikely(!map->map))
		return_error(-errno, NULL, -errno);

	return (error_none);
}

int	_hash_map_destroy(
	t_hash_map *const map
)
{
	for (size_t i = 0; i < map->capacity; i++)
	{
		t_hash_map_entry	*this = map->map[i];
		t_hash_map_entry	*next;

		if (!this)
			continue ;

		next = this;
		do
		{
			this = next;
			next = next->next;
			setting->free(this);
		} while (next);
	}

	return (error_none);
}