/**
 * @file _extract.c
 * @brief Internal TOML tree lookup helpers.
/* ----| Headers    |----- */
	/* Standard */
#include <stdlib.h>
#include <string.h>
 
	/* Internal */
#include "_toml.h"
 
	/* External */
		//...

/* ----| Prototypes |----- */
	//...
 
/* ----| Internals  |----- */

static TOML	*_toml_get_array_index(
	TOML *node,
	const int index
)
{
	int	i = 0;

	while (node && i < index)
	{
		node = node->next;
		i++;
	}

	return (node);
}

/* ----| Public     |----- */

TOML	*_toml_find_child(
	TOML *const parent,
	const char *const key
)
{
	TOML	*node;

	if (unlikely(!parent || !key))
		return (NULL);

	node = parent->child;
	while (node)
	{
		if (node->key && strcmp(node->key, key) == 0)
			return (node);
		node = node->next;
	}

	return (NULL);
}


TOML	*_toml_get_field(
	TOML *const toml,
	const char *const field,
	const int depth
)
{
	char	**split = NULL;
	TOML	*current = NULL;
	size_t	limit = 0;
	size_t	i = 0;

	if (unlikely(!toml || !field))
		return (NULL);

	split = _toml_split(field);
	if (unlikely(!split))
		return (NULL);

	limit = depth < 0 ? _toml_split_len(split) : (size_t)depth;
	current = toml;
	i = 0;
	while (current && i < limit)
	{
		if (current->type == toml_tok_array)
		{
			current = _toml_get_array_index(current->child, atoi(split[i]));
			i++;
		}
		else
		{
			current = _toml_find_child(current, split[i]);
			i++;
		}
	}

	mem_free(split);
	return (current);
}
