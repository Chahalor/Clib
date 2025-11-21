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

int	_json_set_nbr(
	JSON *_json,
	const char *const restrict _field,
	const void *_value
)
{
	char	*_data = NULL;
	int		_errnum = error_none;

	if (!_json->content)
	{
		_json->content = _json_new_content(NULL, json_tok_obj, NULL);
		if (unlikely(!_json->content))
			return (error_alloc_fail);
	}
	if (_value)
		_data = _json_tool_itoa((long long)(*(int *)(_value)));
	else
		return (error_invalid_arg);
	printf("%s:%d : _data='%s'\n", __FILE__, __LINE__, _data);
	_errnum = _json_set_field(_json->content, _field, _data, json_tok_nbr);
	mem_free(_data);
	return (_errnum);
}

int	_json_set_str(
	JSON *_json,
	const char *const restrict _field,
	const void *_value
)
{
	if (!_json->content)
	{
		_json->content = _json_new_content(NULL, json_tok_obj, NULL);
		if (unlikely(!_json->content))
			return (error_alloc_fail);
	}
	return (_json_set_field(_json->content, _field, _value, json_tok_str));
}

int	_json_set_array(
	JSON *_json,
	const char *const restrict _field,
	const void *_value
)
{
	if (!_json->content)
	{
		_json->content = _json_new_content(NULL, json_tok_obj, NULL);
		if (unlikely(!_json->content))
			return (error_alloc_fail);
	}
	return (_json_set_field(_json->content, _field, _value, json_tok_array));
}

int	_json_set_obj(
	JSON *_json,
	const char *const restrict _field,
	const void *_value
)
{
	if (!_json->content)
	{
		_json->content = _json_new_content(NULL, json_tok_obj, NULL);
		if (unlikely(!_json->content))
			return (error_alloc_fail);
	}
	return (_json_set_field(_json->content, _field, _value, json_tok_obj));
}

int	_json_set_bool(
	JSON *_json,
	const char *const restrict _field,
	const void *_value
)
{
	int	_bool = 0;

	if (likely(_value))
		_bool = *(int *)(_value) != 0;
	return (_json_set_field(_json->content, _field,
			_bool
				? "true"
				: "false", json_tok_bool));
}

int	_json_set_null(
	JSON *_json,
	const char *const restrict _field
)
{
	return (_json_set_field(_json->content, _field, NULL, json_tok_null));
}


/* ----| |---- */


int	_json_add_child(
	t_json **_target,
	t_json *_child
)
{
	t_json	*_current;

	if (unlikely(!_target))
		return (error_invalid_arg);
	else if (unlikely(!(*_target)))
	{
		*_target = _child;
		return (error_none);
	}
	else if (!(*_target)->child)
		(*_target)->child = _child;
	else
	{
		_current = (*_target)->child;
		while (_current && _current->next)
			_current = _current->next;
		_current->next = _child;
	}
	return (error_none);
}

int	_json_add_next(
	t_json **_target,
	t_json *_next
)
{
	t_json	*_current;

	if (unlikely(!_target))
		return (error_invalid_arg);
	else if (unlikely(!(*_target)))
	{
		*_target = _next;
		return (error_none);
	}
	else if (!(*_target)->next)
		(*_target)->next = _next;
	else
	{
		_current = (*_target)->next;
		while (_current && _current->next)
			_current = _current->next;
		_current->next = _next;
	}
	return (error_none);
}

int	_json_set_field(
	t_json *_json,
	const char *const restrict _field,
	const char *const restrict _data,
	const int _type
)
{
	t_json	*_target = _json_get_field(_json, _field, -1);
	char	**_split = NULL;
	int		_i = 0;
	t_json	*_dummy = NULL;

	if (!_target)
	{
		_split = _json_tool_split(_field);
		if (unlikely(!_split))
			return (error_alloc_fail);

		_target = _json;
		while (_split[_i])
		{
			_dummy = _json_get_field(_target, _split[_i], -1);
			if (!_dummy)
				break;
			_target = _dummy;
			_i++;
		}

		while (_split[_i])
		{
			const int	is_last = (_split[_i + 1] == NULL);

			_dummy = _json_new_content(
				_split[_i],
				is_last
					? _type
					: json_tok_obj,
				NULL
			);

			if (unlikely(!_dummy))
			{
				mem_free(_split);
				return (error_alloc_fail);
			}

			_json_add_child(&_target, _dummy);
			_target = _dummy;
			_i++;
		}
		mem_free(_split);
	}
	else
		mem_free(_target->data);
	_target->data = _data
						? mem_dup(_data, strlen(_data) + 1)
						: NULL;
	_target->type = _type;
	return (error_none);
}

int	_json_set(
	JSON *_json,
	const char *const restrict _field,
	const void *_value,
	const int _type
)
{
	switch (_type)
	{
		case (json_tok_nbr):
			return (_json_set_nbr(_json, _field, _value));
		case (json_tok_str):
			return (_json_set_str(_json, _field, _value));
		case (json_tok_array):
			return (_json_set_array(_json, _field, _value));
		case (json_tok_obj):
			return (_json_set_obj(_json, _field, _value));
		case (json_tok_bool):
			return (_json_set_bool(_json, _field, _value));
		case (json_tok_null):
			return (_json_set_null(_json, _field));
		default:
			return (error_invalid_arg);
	}
}

int	_json_unset(
	JSON *_json,
	const char *const restrict _field
)
{
	t_json	*_target = _json_get_field(_json->content, _field, -1);
	t_json	*_prev = _json_get_field(_json->content, _field, _json_tool_count_field(_field) - 1);
	t_json	*_next = NULL;

	printf("%s: _target=%p, _prev=%p\n", __func__, _target, _prev);
	printf("%s: target.key='%s'\n", __func__, _target->key);
	printf("%s: prev.key='%s'\n", __func__, _prev->key);
	if (!_target)
		return (error_none);
	else
	{
		_next = _target->next;
		_json_free_content(_target);
		if (_prev)
			_prev->next = _next;
	}
	return (error_none);
}
