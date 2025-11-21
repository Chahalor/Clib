// Header

/* ----| Headers    |----- */
	/* Standard */
		//...
	/* Internal */
#include "_json.h"
	/* External */
#include "formating.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

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

	while (_current && _i < _limits)
	{
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

char	*_json_access_field(
	t_json *_json,
	const char *const restrict _field,
	const int _depth
)
{
	t_json	*_node = NULL;

	_node = _json_get_field(_json, _field, _depth);
	if (likely(_node))
		return (_node->data);
	else
		return (NULL);
}
