// Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdlib.h>
#include <string.h>

	/* Internal */
#include "_json.h"
	/* External */
#include "formating.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */


static inline t_json	*_json_get_field_array(
	t_json	*_node,
	const int _index
)
{
	int		_depth = 0;
	t_json	*_current;

	_current = _node;
	while (_current->next && _depth < _index)
	{
		_current = _current->next;
		_depth++;
	}
	return (_current);
}

t_json	*_json_get_field(
	t_json *_json,
	const char *const restrict _field,
	const int _depth
)
{
	char	**_split = NULL;
	t_json	*_current = NULL;
	int		_i = 0;
	int		_limits = 0;

	if (unlikely(!_json || !_json->child))
		return (NULL);
	else
		_current = _json->child;

	_split = _json_tool_split(_field);
	if (unlikely(!_split))
		return (NULL);

	_i = 0;
	if (_depth < 0)
		_limits = _json_tool_array_len(_split, sizeof(char *));
	else
		_limits = _depth;

	while (_current && _i < _limits && _current->type != json_tok_array)
	{
		if (_current->type == json_tok_array)
		{
			int	_idx = atoi(_split[_i]);
			_current = _json_get_field_array(_current, _idx);
		}

		while (_current && strcmp(_current->key, _split[_i]))
			_current = _current->next;

		if (unlikely(!_current))
			break ;
		else if (_i + 1 < _limits)
			_current = _current->child;
		_i++;
	}

	mem_free(_split);
	return (_current);
}

void	*_json_access_field(
	t_json *_json,
	const char *const restrict _field,
	const int _depth
)
{
	t_json	*_node = NULL;

	_node = _json_get_field(_json, _field, _depth);
	if (unlikely(!_node))
		return (NULL);
	else if (_node->type != json_tok_array)
		return (_node->data);
	else
		return (_node);
}

size_t	_json_node_size(
	const t_json *_node
)
{
	size_t	size = 0;

	if (!_node)
		return (0);

	size += sizeof(t_json);

	if (_node->key)
		size += strlen(_node->key) + 1;

	if (_node->data)
		size += strlen(_node->data) + 1;

	size += _json_node_size(_node->child);
	size += _json_node_size(_node->next);

	return (size);
}

size_t	_json_access_len(
	const t_json *_json
)
{
	size_t	count;
	t_json	*cur;

	if (unlikely(!_json || !_json->data))
		return (0);

	if (_json->type != json_tok_array &&
		_json->type != json_tok_obj)
		return (0);

	count = 0;
	cur = _json->child;

	while (cur)
	{
		count++;
		cur = cur->next;
	}
	return (count);
}
