/**
 * @file _memory.c
 * @brief Internal TOML allocation and cloning helpers.
*/
/* ----| Headers    |----- */
	/* Standard */
#include <string.h>

	/* Internal */
#include "_toml.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

TOML	*_toml_new_content(
	const char *const key,
	const int type,
	char *const data
)
{
	TOML	*result = NULL;

	result = mem_alloc(sizeof(TOML));
	if (unlikely(!result))
		return (NULL);

	memset(result, 0, sizeof(TOML));
	if (key)
	{
		result->key = mem_dup(key, strlen(key) + 1);
		if (unlikely(!result->key))
		{
			mem_free(result);
			return (NULL);
		}
	}

	result->type = type;
	result->data = data;
	return (result);
}

void	_toml_free_content(
	TOML *node
)
{
	if (unlikely(!node))
		return ;

	mem_free(node->key);
	mem_free(node->data);
	_toml_free_content(node->child);
	_toml_free_content(node->next);
	mem_free(node);
}

TOML	*_toml_clone_node(
	const TOML *const src
)
{
	TOML	*node;
	char	*data;

	if (unlikely(!src))
		return (NULL);

	data = NULL;
	if (src->data)
	{
		data = mem_dup(src->data, strlen(src->data) + 1);
		if (unlikely(!data))
			return (NULL);
	}

	node = _toml_new_content(src->key, src->type, data);
	if (unlikely(!node))
	{
		mem_free(data);
		return (NULL);
	}
	else if (src->child)
	{
		node->child = _toml_clone_node(src->child);
		if (unlikely(!node->child))
			goto error;
	}

	if (src->next)
	{
		node->next = _toml_clone_node(src->next);
		if (unlikely(!node->next))
			goto error;
	}

	return (node);

error:
	_toml_free_content(node);
	return (NULL);
}
