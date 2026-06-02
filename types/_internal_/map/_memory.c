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

	while (this && this->next)
		this = this->next;

	if (!this)
		map->map[pos] = entry;
	else
		this->next = entry;

	this->hash = hash;
	map->length++;
}

void	_hash_map_free(
	t_hash_map *const	map,
	const char *const	key,
	const bool			should_free
)
{
	const size_t		pos = setting->hash_string(key) % map->capacity;
	t_hash_map_entry	*this = _entry_find(map, key);
	t_hash_map_entry	*prev = _entry_find_prev(map, key);

	if (unlikely(!this))
		return ;

	if (prev)
		prev->next = this->next;

	if (should_free)
		setting->free(this->value);

	setting->free(this);
}
