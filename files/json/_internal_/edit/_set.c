// Header

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>
#include <stdint.h>
#include <stddef.h>

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
	t_json			*_target = NULL;
	t_json			*_clone = NULL;
	int				result = error_none;

	if (unlikely(!_json || !_field))
		return (error_invalid_arg);
	if (!*_json)
	{
		*_json = _json_new_content(NULL, json_tok_obj, NULL);
		if (unlikely(!*_json))
			return (error_alloc_fail);
	}

	result = _json_set_field(_json, _field, NULL, _type);
	if (unlikely(result != error_none))
		goto error;

	_target = _json_get_field(*_json, _field, -1);
	if (unlikely(!_target))
	{
		result = error_invalid_arg;
		goto error;
	}

	if (_value)
	{
		_src = (const t_json *)_value;
		if (unlikely(_src->type != _type))
		{
			result = error_invalid_arg;
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
		goto error;
	}

	if (_src->child)
	{
		_clone = _json_clone_node(_src->child);
		if (unlikely(!_clone))
		{
			result = error_alloc_fail;
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
	return (result);
}

static int	_json_set_nbr(
	JSON **_json,
	const char *const restrict _field,
	const void *_value
)
{
	char	*_data = NULL;
	int		result = error_none;

	if (unlikely(!_json || !_field || !_value))
		return (error_invalid_arg);
	if (!*_json)
	{
		*_json = _json_new_content(NULL, json_tok_obj, NULL);
		if (unlikely(!*_json))
			return (error_alloc_fail);
	}

	_data = _json_tool_itoa((long long)(*(const int *)_value));
	if (unlikely(!_data))
		return (error_alloc_fail);

	result = _json_set_field(_json, _field, _data, json_tok_nbr);
	mem_free(_data);
	return (result);
}

static int	_json_set_unbr(
	JSON **_json,
	const char *const restrict _field,
	const void *_value
)
{
	char	*_data = NULL;
	int		result = error_none;

	if (unlikely(!_json || !_field || !_value))
		return (error_invalid_arg);
	if (!*_json)
	{
		*_json = _json_new_content(NULL, json_tok_obj, NULL);
		if (unlikely(!*_json))
			return (error_alloc_fail);
	}

	_data = _json_tool_unsigned_itoa(*(const unsigned long long *)_value);
	if (unlikely(!_data))
		return (error_alloc_fail);

	result = _json_set_field(_json, _field, _data, json_tok_nbr);
	mem_free(_data);
	return (result);
}

static int	_json_set_number_va_args(
	JSON *const json,
	const char *const field,
	const long long signed_value,
	const unsigned long long unsigned_value,
	const int is_unsigned,
	va_list *const restrict args
)
{
	t_json_str	*str_field = NULL;
	JSON		*json_ref = json;
	char		*data = NULL;
	int		errnum = error_none;

	if (unlikely(!json || !field || !args))
		return (error_invalid_arg);

	str_field = _json_new_str(INTERFACE_JSON_STRING_ALLOC_SIZE);
	if (unlikely(!str_field))
		return (error_alloc_fail);

	errnum = _json_fill_format(field, str_field, args);
	if (unlikely(errnum != error_none))
		goto cleanup;

	data = is_unsigned
			? _json_tool_unsigned_itoa(unsigned_value)
			: _json_tool_itoa(signed_value);
	if (unlikely(!data))
	{
		errnum = error_alloc_fail;
		goto cleanup;
	}

	errnum = _json_set_field(&json_ref, str_field->content, data, json_tok_nbr);

cleanup:
	mem_free(data);
	mem_free(str_field);
	return (errnum);
}

int	_json_set_signed_nb(
	JSON *const json,
	const char *const field,
	const long long var,
	va_list *const restrict args
)
{
	return (_json_set_number_va_args(
		json, field, var,0ULL, 0, args));
}

int	_json_set_unsigned_nb(
	JSON *const json,
	const char *const field,
	const unsigned long long var,
	va_list *const restrict args
)
{
	return (_json_set_number_va_args(
		json, field, 0LL, (unsigned long long)var, 1, args));
}

static void	_json_free_list(
	t_json *_node
)
{
	t_json	*_next;

	while (_node)
	{
		_next = _node->next;
		_node->next = NULL;
		_json_free_content(_node);
		_node = _next;
	}
}

/* ----| Public    |----- */
#pragma region Public

int	_json_set_array(
	JSON **_json,
	const char *const restrict _field,
	const void *_value
)
{
	return (_json_set_container(_json, _field, _value, json_tok_array));
}

int	_json_set_obj(
	JSON **_json,
	const char *const restrict _field,
	const void *_value
)
{
	return (_json_set_container(_json, _field, _value, json_tok_obj));
}

int	_json_set_str(
	JSON **_json,
	const char *const restrict _field,
	const void *_value
)
{
	if (unlikely(!_json || !_field))
		return (error_invalid_arg);
	if (!*_json)
	{
		*_json = _json_new_content(NULL, json_tok_obj, NULL);
		if (unlikely(!*_json))
			return (error_alloc_fail);
	}
	return (_json_set_field(_json, _field, _value, json_tok_str));
}

int	_json_set_bool(
	JSON **_json,
	const char *const restrict _field,
	const void *_value
)
{
	int	_bool = 0;

	if (unlikely(!_json || !_field))
		return (error_invalid_arg);
	if (!*_json)
	{
		*_json = _json_new_content(NULL, json_tok_obj, NULL);
		if (unlikely(!*_json))
			return (error_alloc_fail);
	}

	if (likely(_value))
		_bool = *(const int *)(_value) != 0;
	return (_json_set_field(_json, _field,
			_bool
				? "true"
				: "false", json_tok_bool));
}

int	_json_set_null(
	JSON **_json,
	const char *const restrict _field
)
{
	if (unlikely(!_json || !_field))
		return (error_invalid_arg);
	if (!*_json)
	{
		*_json = _json_new_content(NULL, json_tok_obj, NULL);
		if (unlikely(!*_json))
			return (error_alloc_fail);
	}
	return (_json_set_field(_json, _field, NULL, json_tok_null));
}

int	_json_set_field(
	t_json **_json,
	const char *const restrict _field,
	const char *const restrict _data,
	const int _type
)
{
	t_json	*_target = NULL;
	char	**_split = NULL;
	t_json	*_dummy = NULL;
	t_json	*_created_parent = NULL;
	t_json	*_created_root = NULL;
	char	*_new_data = NULL;
	int		_i = 0;
	int		result = error_none;

	if (unlikely(!_json || !_field))
		return (error_invalid_arg);

	if (!*_json)
	{
		*_json = _json_new_content(NULL, json_tok_obj, NULL);
		if (unlikely(!*_json))
			return (error_alloc_fail);
	}

	_target = _json_get_field(*_json, _field, -1);
	if (!_target)
	{
		_split = _json_tool_split(_field);
		if (unlikely(!_split))
		{
			result = error_alloc_fail;
			goto cleanup;
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
				result = error_alloc_fail;
				goto cleanup;
			}

			if (!_created_root)
			{
				_created_parent = _target;
				_created_root = _dummy;
			}

			result = _json_add_child(&_target, _dummy);
			if (unlikely(result != error_none))
			{
				_dummy->next = NULL;
				_json_free_content(_dummy);
				goto cleanup;
			}

			_target = _dummy;
			_i++;
		}
	}

	if (unlikely(!_target))
	{
		result = error_invalid_arg;
		goto cleanup;
	}

	if (_data)
	{
		_new_data = mem_dup(_data, strlen(_data) + 1);
		if (unlikely(!_new_data))
		{
			result = error_alloc_fail;
			goto cleanup;
		}
	}

	mem_free(_target->data);
	_target->data = _new_data;
	_new_data = NULL;
	_target->type = _type;

cleanup:
	if (result != error_none)
	{
		mem_free(_new_data);
		if (_created_parent && _created_root)
			_json_remove_node(&_created_parent->child, _created_root);
	}
	mem_free(_split);
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
		case (json_tok_unbr):
			result = _json_set_unbr(_json, _field, _value);
			break ;
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

	if (unlikely(!_json || !_field || !_args))
		return (error_invalid_arg);
	if (unlikely(_type == json_tok_str && !_value))
		return (error_invalid_arg);

	_str_field = _json_new_str(INTERFACE_JSON_STRING_ALLOC_SIZE);
	if (unlikely(!_str_field))
		return (error_alloc_fail);

	errnum = _json_fill_format(_field, _str_field, _args);
	if (unlikely(errnum != error_none))
		goto error;

	if (_type == json_tok_str)
	{
		_str_value = _json_new_str(INTERFACE_JSON_STRING_ALLOC_SIZE);
		if (unlikely(!_str_value))
		{
			errnum = error_alloc_fail;
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
					_str_value
						? _str_value->content
						: _value,
					_type
				);

error:
	mem_free(_str_field);
	mem_free(_str_value);
	return (errnum);
}

int	_json_set_array_va_args(
	JSON *_json,
	const char *const _field,
	void *const _array,
	const size_t _length,
	const int _type,
	...
)
{
	va_list	_args;
	int		errnum;

	va_start(_args, _type);
	errnum = _json_set_array_va_list(&_json, _field, _array, _length, _type, &_args);
	va_end(_args);
	return (errnum);
}

int	_json_append_array(
	JSON *_json,
	void *const _value,
	const int _type
)
{
	char	*_data = NULL;
	JSON	*_node = NULL;
	int		errnum = error_none;

	switch (_type)
	{
		case (json_tok_nbr):
		{
			const int	_val = *((const int *)_value);

			_data = _json_tool_itoa((long long)_val);
			if (unlikely(!_data))
			{
				errnum = error_alloc_fail;
				goto cleanup;
			}

			break ;
		}

		case (json_tok_str):
		{
			const char	*_val = (const char *)_value;

			_data = _val ?
							mem_dup(_val, strlen(_val) + 1) :
							NULL;
				if (unlikely(_val && !_data))
			{
				errnum = error_alloc_fail;
				goto cleanup;
			}

			break ;
		}

		case (json_tok_bool):
		{
			const int	_val = *((const int *)_value);

			_data = _val ?
							mem_dup("true", sizeof("true")) :
							mem_dup("false", sizeof("false"));
			if (unlikely(!_data))
			{
				errnum = error_alloc_fail;
				goto cleanup;
			}

			break ;
		}

		case (json_tok_array):
		case (json_tok_obj):
		{
			JSON	*_val = (t_json *)_value;

			_json_add_child(&_json, _val);
			goto cleanup;
		}

		case (json_tok_null):
			break;

		default:
			errnum = error_invalid_arg;
			goto cleanup;
	}

	_node = _json_new_content(NULL, _type, _data);
	if (unlikely(!_node))
	{
		if (_type != json_tok_obj && _type != json_tok_array)
			mem_free(_data);
		errnum = error_alloc_fail;
		goto cleanup;
	}

	_json_add_child(&_json, _node);

cleanup:
	return (errnum);
}

int	_json_pop_array(
	JSON *const _array,
	const size_t _index
)
{
	JSON	*_prev = NULL;
	JSON	*_curr;
	size_t	_i = 0;

	_curr = _array->child;

	while (_curr && _i < _index)
	{
		_prev = _curr;
		_curr = _curr->next;
		_i++;
	}

	if (!_curr)
		return (-1);

	if (_prev)
		_prev->next = _curr->next;
	else
		_array->child = _curr->next;

	_curr->next = NULL;
	_json_free_all(_curr);
	return (0);
}

int	_json_set_array_va_list(
	JSON **_json,
	const char *const restrict _field,
	void *const _array,
	const size_t _length,
	const int _type,
	va_list *const restrict _args
)
{
	t_json_str	*_str_field = NULL;
	t_json		*_array_node = NULL;
	size_t		_i = 0;
	int			errnum = error_none;

	if (unlikely(!_json || !_field || !_args))
		return (error_invalid_arg);

	_str_field = _json_new_str(INTERFACE_JSON_STRING_ALLOC_SIZE);
	if (unlikely(!_str_field))
		return (error_alloc_fail);

	errnum = _json_fill_format(_field, _str_field, _args);
	if (unlikely(errnum != error_none))
		goto cleanup;

	_array_node = _json_new_content(NULL, json_tok_array, NULL);
	if (unlikely(!_array_node))
	{
		errnum = error_alloc_fail;
		goto cleanup;
	}

	if (unlikely(!_array && _type != json_tok_null && _length > 0))
	{
		errnum = error_invalid_arg;
		goto cleanup;
	}

	while (_i < _length)
	{
		t_json	*_node = NULL;
		char	*_data = NULL;

		switch (_type)
		{
			case (json_tok_str):
			{
				const char	*_src = ((const char *const *)_array)[_i];

				_data = _src ?
							mem_dup(_src, strlen(_src) + 1) :
							NULL;
				if (unlikely(_src && !_data))
				{
					errnum = error_alloc_fail;
					goto cleanup;
				}
				break ;
			}

			case (json_tok_nbr):
			{
				const int	_val = ((const int *)_array)[_i];

				_data = _json_tool_itoa((long long)_val);
				if (unlikely(!_data))
				{
					errnum = error_alloc_fail;
					goto cleanup;
				}
				break ;
			}

			case (json_tok_bool):
			{
				const int	_val = ((const int *)_array)[_i];

				_data = _val ?
							mem_dup("true", sizeof("true")) :
							mem_dup("false", sizeof("false"));
				if (unlikely(!_data))
				{
					errnum = error_alloc_fail;
					goto cleanup;
				}
				break ;
			}

			case (json_tok_obj):
			case (json_tok_array):
			{
				const t_json	*_src = ((t_json *const *)_array)[_i];

				if (unlikely(!_src || _src->type != _type))
				{
					errnum = error_invalid_arg;
					goto cleanup;
				}

				_node = _json_new_content(NULL, _type, NULL);
				if (unlikely(!_node))
				{
					errnum = error_alloc_fail;
					goto cleanup;
				}

				if (_src->child)
				{
					_node->child = _json_clone_node(_src->child);
					if (unlikely(!_node->child))
					{
						_json_free_content(_node);
						errnum = error_alloc_fail;
						goto cleanup;
					}
				}
				break ;
			}

			case (json_tok_null):
				break ;

			default:
				errnum = error_invalid_arg;
				goto cleanup;
		}

		if (!_node)
		{
			_node = _json_new_content(NULL, _type, _data);
			if (unlikely(!_node))
			{
				mem_free(_data);
				errnum = error_alloc_fail;
				goto cleanup;
			}
		}

		errnum = _json_add_child(&_array_node, _node);
		if (unlikely(errnum != error_none))
		{
			_node->next = NULL;
			_json_free_content(_node);
			goto cleanup;
		}

		_i++;
	}

	errnum = _json_set_array(_json, _str_field->content, _array_node);

cleanup:
	if (_array_node)
	{
		_json_free_list(_array_node->child);
		_array_node->child = NULL;
		_json_free_content(_array_node);
	}
	mem_free(_str_field);
	return (errnum);
}
