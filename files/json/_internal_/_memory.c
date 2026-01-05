// Header

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>

	/* Internal */
#include "_json.h"
	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

t_json	*_json_new_content(
	const char *_key,
	int _type,
	char *_data
)
{
	t_json	*result = NULL;
	
	result = mem_alloc(sizeof(t_json));
	if (unlikely(!result))
		return (NULL);
	result->key = _key ?
		mem_dup(_key, strlen(_key) + 1) :
		NULL;
	result->type = _type;
	result->data = _data;
	result->child = NULL;
	result->next = NULL;
	return (result);
}

void	_json_free_content(
	t_json *_target
)
{
	if (unlikely(!_target))
		return ;
	if (_target->key)
		mem_free(_target->key);
	if (_target->data)
		mem_free(_target->data);
	_json_free_content(_target->child);
	mem_free(_target);
}

int	_json_remove_node(
	t_json **_head,
	t_json *_target
)
{
	t_json	*_curr;
	t_json	*_prev;

	if (unlikely(!_head || !*_head || !_target))
		return (error_invalid_arg);

	_curr = *_head;
	_prev = NULL;

	while (_curr)
	{
		if (_curr == _target)
		{
			if (_prev)
				_prev->next = _curr->next;
			else
				*_head = _curr->next;

			_json_free_content(_curr);
			return (error_none);
		}
		_prev = _curr;
		_curr = _curr->next;
	}
	return (error_none);
}

int	_json_free_all(
	JSON *_target
)
{
	t_json	*_content = NULL;
	int		error = error_none;

	if (unlikely(!_target))
		return (error_invalid_arg);
	else
		_content = _target;
	_json_free_content(_content);
	return (error);
}

/**
 * @brief	clone the current node by duplicating it recursivly
 */
t_json	*_json_clone_node(
	const t_json *_src
)
{
	t_json	*node = NULL;
	char	*_data = NULL;

	if (!_src)
		goto error;
	else if (_src->data)
	{
		_data = mem_dup(_src->data, strlen(_src->data) + 1);
		if (unlikely(!_data))
			goto error;
	}

	node = _json_new_content(_src->key, _src->type, _data);
	if (unlikely(!node))
		goto error;
	else if (_src->child)
	{
		node->child = _json_clone_node(_src->child);
		if (unlikely(!node->child))
		{
			node = NULL;
			goto error;
		}
	}
	else if (_src->next)
	{
		node->next = _json_clone_node(_src->next);
		if (unlikely(!node->next))
		{
			node = NULL;
			goto error;
		}
	}

	return (node);

error:
	mem_free(_data);
	_json_free_content(node);
	return (node);
}