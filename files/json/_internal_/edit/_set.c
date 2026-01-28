// Header

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>
#include <stdint.h>

	/* Internal */
#include "_json_edit.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internal  |----- */

static int	_json_set_container(
	JSON **_json,
	const char *const restrict _field,
	const void *_value,
	const int _type
)
{
	const t_json	*_src = NULL;
	t_json			*_target;
	t_json			*_clone = NULL;
	int				result;

	if (!*_json)
	{
		*_json = _json_new_content(NULL, json_tok_obj, NULL);
		if (unlikely(!*_json))
		{
			result = -(error_alloc_fail);
			goto error;
		}
	}

	result = _json_set_field(_json, _field, NULL, _type);
	if (unlikely(result != error_none))
		goto error;

	_target = _json_get_field(*_json, _field, -1);
	if (unlikely(!_target))
	{
		result = -(error_invalid_arg);
		goto error;
	}

	if (_value)
	{
		_src = (const t_json *)_value;
		if (unlikely(_src->type != _type))
		{
			result = -(error_invalid_arg);
			goto error;
		}
	}
	else
	{
		if (_target->child)
		{
			_json_free_content(_target->child);
			_target->child = NULL;
		}
		result = error_none;
		goto error;
	}

	if (_src->child)
	{
		_clone = _json_clone_node(_src->child);
		if (unlikely(!_clone))
		{
			result = -(error_alloc_fail);
			goto error;
		}
	}
	if (_target->child)
	{
		_json_free_content(_target->child);
		_target->child = NULL;
	}
	_target->child = _clone;

error:
	return (error_none);
}

static int	_json_set_array(
	JSON **_json,
	const char *const restrict _field,
	const void *_value
)
{
	return (_json_set_container(_json, _field, _value, json_tok_array));
}

static int	_json_set_obj(
	JSON **_json,
	const char *const restrict _field,
	const void *_value
)
{
	return (_json_set_container(_json, _field, _value, json_tok_obj));
}

static int	_json_set_nbr(
	JSON **_json,
	const char *const restrict _field,
	const void *_value
)
{
	char	*_data = NULL;
	int		result = error_none;

	if (!*_json)
	{
		*_json = _json_new_content(NULL, json_tok_obj, NULL);
		if (unlikely(!_json))
			return (error_alloc_fail);
	}
	if (_value)
		_data = _json_tool_itoa((long long)(*(int *)(_value)));
	else
		return (error_invalid_arg);
	result = _json_set_field(_json, _field, _data, json_tok_nbr);
	mem_free(_data);
	return (result);
}

static int	_json_set_str(
	JSON **_json,
	const char *const restrict _field,
	const void *_value
)
{
	if (!*_json)
	{
		*_json = _json_new_content(NULL, json_tok_obj, NULL);
		if (unlikely(!*_json))
			return (error_alloc_fail);
	}
	return (_json_set_field(_json, _field, _value, json_tok_str));
}

static int	_json_set_bool(
	JSON **_json,
	const char *const restrict _field,
	const void *_value
)
{
	int	_bool = 0;

	if (likely(_value))
		_bool = *(int *)(_value) != 0;
	return (_json_set_field(_json, _field,
			_bool
				? "true"
				: "false", json_tok_bool));
}

static int	_json_set_null(
	JSON **_json,
	const char *const restrict _field
)
{
	return (_json_set_field(_json, _field, NULL, json_tok_null));
}

/* ----| Public    |----- */

int	_json_set_field(
	t_json **_json,
	const char *const restrict _field,
	const char *const restrict _data,
	const int _type
)
{
	t_json	*_target = _json_get_field(*_json, _field, -1);
	char	**_split = NULL;
	int		_i = 0;
	t_json	*_dummy = NULL;
	int		result = error_none;

	if (!_target)
	{
		_split = _json_tool_split(_field);
		if (unlikely(!_split))
		{
			result = -(error_alloc_fail);
			goto error;
		}

		_target = *_json;
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
				result = -(error_alloc_fail);
				goto error;
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

error:
	return (result);
}

int	_json_set(
	JSON **_json,
	const char *const restrict _field,
	const void *_value,
	const int _type
)
{
	int	result;

	switch (_type)
	{
		case (json_tok_nbr):
			result = (_json_set_nbr(_json, _field, _value));
			break;
		case (json_tok_str):
			result = (_json_set_str(_json, _field, _value));
			break;
		case (json_tok_array):
			result = (_json_set_array(_json, _field, _value));
			break;
		case (json_tok_obj):
			result = (_json_set_obj(_json, _field, _value));
			break;
		case (json_tok_bool):
			result = (_json_set_bool(_json, _field, _value));
			break;
		case (json_tok_null):
			result = (_json_set_null(_json, _field));
			break;
		default:
			result = (error_invalid_arg);
	}

	return (result);
}

/**
 * for type we should be able to do thing like that:
 * json_tok_array | json_tok_str --> this mean i pass has param a null terminated array of char *
 * json_tok_array | json_tok_nbr --> here i pass
*/
int	_json_set_va_args(
	JSON **_json,
	const char *const restrict _field,
	const void *_value,
	const unsigned int _type,
	va_list *const restrict _args
)
{
	t_json_str	*_str_field = NULL;
	t_json_str	*_str_value = NULL;
	int			errnum = error_none;


	_str_field = _json_new_str(INTERFACE_JSON_STRING_ALLOC_SIZE);
	if (unlikely(!_str_field))
	{
		errnum = -(error_alloc_fail);
		goto error;
	}

	errnum = _json_fill_format(_field, _str_field, _args);
	if (unlikely(errnum != error_none))
		goto error;

	if (_type == json_tok_str)
	{
		_str_value = _json_new_str(INTERFACE_JSON_STRING_ALLOC_SIZE);
		if (unlikely(!_str_value))
		{
			errnum = -(error_alloc_fail);
			goto error;
		}
		errnum = _json_fill_format((const char *const restrict)_value,
									_str_value,
									_args
								);
		if (unlikely(errnum != error_none))
			goto error;
	}

	errnum = _json_set(_json,
						_str_field->content,
						_str_value ?
							_str_value->content :
							_value,
						_type
					);

error:
	mem_free(_str_field);
	mem_free(_str_value);
	return (errnum);
}
