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

t_hash_map_entry	*_entry_find(
	const t_hash_map *const	map,
	const char *const		key
)
{
	const size_t		hash = setting->hash_string(key);
	const size_t		pos = _POS(map, hash);
	t_hash_map_entry	*this = map->map[pos];

	while (this)
	{
		if (this->hash == hash)
		{
			if (!strcmp(this->key, key))
				break ;
		}

		this = this->next;
	}

	return (this);
}

t_hash_map_entry	*_entry_find_prev(
	t_hash_map *const	map,
	const char *const	key
)
{
	const size_t		hash = setting->hash_string(key);
	const size_t		pos = _POS(map, hash);
	t_hash_map_entry	*this = map->map[pos];
	t_hash_map_entry	*prev = NULL;

	while (this)
	{
		if (this->hash == hash)
		{
			if (!strcmp(this->key, key))
				return (prev);
		}

		prev = this;
		this = this->next;
	}

	return (NULL);
}
