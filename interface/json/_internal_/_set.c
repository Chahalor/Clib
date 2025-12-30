// Header

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>
#include <stdint.h>
	/* Internal */
#include "_json.h"
	/* External */
		//...

/* ----| Prototypes |----- */

static int	_json_set_fill_format_with_first(const char *const restrict _str, t_json_str *const restrict _result, va_list *const restrict _args, const void *const first_arg);

/* ----| Internals  |----- */

static int	_json_set_fill_format_with_first(
	const char *const restrict _str,
	t_json_str *const restrict _result,
	va_list *const restrict _args,
	const void *const first_arg
)
{
	size_t	_i = 0;
	int		_errnum = error_none;
	int		_use_first = 1;

	if (unlikely(!_str || !_result || !_args))
		return (error_invalid_arg);
	while (_str[_i])
	{
		if (_str[_i] != '%')
		{
				_errnum = _json_str_append_char(_result, _str[_i]);
			if (unlikely(_errnum != error_none))
				return (_errnum);
			_i++;
			continue;
		}
		if (_str[_i + 1] == '\0')
			return (_json_str_append_char(_result, '%'));
		if (_str[_i + 1] == '%')
		{
			_errnum = _json_str_append_char(_result, '%');
			if (unlikely(_errnum != error_none))
				return (_errnum);
			_i += 2;
			continue;
		}
		if (_str[_i + 1] == 'l')
		{
			const char	_spec = _str[_i + 2];

			if (_spec == 'd' || _spec == 'i')
			{
				long long	_value;

				if (_use_first)
					_value = (long long)(intptr_t)first_arg;
				else
					_value = va_arg(*_args, long);
				_use_first = 0;
					_errnum = _json_add_value(_result, &_value, _json_format_type_long);
				if (unlikely(_errnum != error_none))
					return (_errnum);
				_i += 3;
				continue;
			}
			if (_spec == 'x')
			{
				unsigned long long	_value;

				if (_use_first)
					_value = (unsigned long long)(uintptr_t)first_arg;
				else
					_value = va_arg(*_args, unsigned long);
				_use_first = 0;
					_errnum = _json_add_value(_result, &_value, _json_format_type_hex);
				if (unlikely(_errnum != error_none))
					return (_errnum);
				_i += 3;
				continue;
			}
		}
		{
			const char	c = _str[_i + 1];

			switch (c)
			{
			case 'd':
			case 'i':
			{
				int	_value;

				if (_use_first)
					_value = (int)(intptr_t)first_arg;
				else
					_value = va_arg(*_args, int);
				_use_first = 0;
					_errnum = _json_add_value(_result, &_value, _json_format_type_int);
				break;
			}
			case 's':
			{
				const char	*_s;

				if (_use_first)
					_s = (const char *)first_arg;
				else
					_s = va_arg(*_args, char *);
				_use_first = 0;
					_errnum = _json_add_value(_result, _s, _json_format_type_str);
				break;
			}
			case 'p':
			{
				unsigned long long	_value;

				if (_use_first)
					_value = (unsigned long long)(uintptr_t)first_arg;
				else
					_value = (unsigned long long)(uintptr_t)va_arg(*_args, void *);
				_use_first = 0;
					_errnum = _json_str_append_n(_result, "0x", 2);
				if (unlikely(_errnum != error_none))
					return (_errnum);
					_errnum = _json_add_value(_result, &_value, _json_format_type_hex);
				break;
			}
			case 'x':
			{
				unsigned long long	_value;

				if (_use_first)
					_value = (unsigned long long)(uintptr_t)first_arg;
				else
					_value = va_arg(*_args, unsigned int);
				_use_first = 0;
					_errnum = _json_add_value(_result, &_value, _json_format_type_hex);
				break;
			}
			case 'c':
			{
				char	_ch;

				if (_use_first)
					_ch = (char)(intptr_t)first_arg;
				else
					_ch = (char)va_arg(*_args, int);
				_use_first = 0;
					_errnum = _json_add_value(_result, &_ch, _json_format_type_char);
				break;
			}
			default:
				_errnum = _json_str_append_char(_result, '%');
				if (unlikely(_errnum != error_none))
					return (_errnum);
				_errnum = _json_str_append_char(_result, c);
				break;
			}
		}
		if (unlikely(_errnum != error_none))
			return (_errnum);
		_i += 2;
	}
	return (error_none);
}

static t_json	*_json_clone_node(
	const t_json *src
)
{
	t_json	*node;
	char	*data = NULL;

	if (!src)
		return (NULL);
	if (src->data)
	{
		data = mem_dup(src->data, strlen(src->data) + 1);
		if (unlikely(!data))
			return (NULL);
	}
	node = _json_new_content(src->key, src->type, data);
	if (unlikely(!node))
		return (mem_free(data), NULL);
	if (src->child)
	{
		node->child = _json_clone_node(src->child);
		if (unlikely(!node->child))
			return (_json_free_content(node), NULL);
	}
	if (src->next)
	{
		node->next = _json_clone_node(src->next);
		if (unlikely(!node->next))
			return (_json_free_content(node), NULL);
	}
	return (node);
}

static int	_json_set_container(
	JSON **_json,
	const char *const restrict _field,
	const void *_value,
	const int _type
)
{
	t_json			*_target;
	const t_json	*_src = NULL;
	t_json			*_clone = NULL;
	int				_errnum;

	if (_value)
	{
		_src = (const t_json *)_value;
		if (unlikely(_src->type != _type))
			return (error_invalid_arg);
	}

	if (!*_json)
	{
		*_json = _json_new_content(NULL, json_tok_obj, NULL);
		if (unlikely(!*_json))
			return (error_alloc_fail);
	}
	_errnum = _json_set_field(_json, _field, NULL, _type);
	if (unlikely(_errnum != error_none))
		return (_errnum);

	_target = _json_get_field(*_json, _field, -1);
	if (unlikely(!_target))
		return (error_invalid_arg);

	if (!_value)
	{
		if (_target->child)
		{
			_json_free_content(_target->child);
			_target->child = NULL;
		}
		return (error_none);
	}

	if (_src->child)
	{
		_clone = _json_clone_node(_src->child);
		if (unlikely(!_clone))
			return (error_alloc_fail);
	}
	if (_target->child)
	{
		_json_free_content(_target->child);
		_target->child = NULL;
	}
	_target->child = _clone;
	return (error_none);
}

int	_json_set_nbr(
	JSON **_json,
	const char *const restrict _field,
	const void *_value
)
{
	char	*_data = NULL;
	int		_errnum = error_none;

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
	_errnum = _json_set_field(_json, _field, _data, json_tok_nbr);
	mem_free(_data);
	return (_errnum);
}

int	_json_set_str(
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

int	_json_set_bool(
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

int	_json_set_null(
	JSON **_json,
	const char *const restrict _field
)
{
	return (_json_set_field(_json, _field, NULL, json_tok_null));
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

	if (!_target)
	{
		_split = _json_tool_split(_field);
		if (unlikely(!_split))
			return (error_alloc_fail);

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
	JSON **_json,
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

int	_json_set_va_args(
	JSON **_json,
	const char *const restrict _field,
	const void *_value,
	va_list *const restrict _args
)
{
	t_json_str	_str = {0};
	int			_type;
	int			errnum = error_none;
	const char	*format = _field;
	char		*format_buf = NULL;
	size_t		field_len = 0;
	size_t		trim_len = 0;
	int			has_format_type = 0;
	int			format_type = 0;
	int			field_has_format = 0;
	const void	*value = _value;

	_str.content = mem_alloc(sizeof(char) * INTERFACE_JSON_STRING_ALLOC_SIZE);
	if (unlikely(!_str.content))
	{
		errnum = error_alloc_fail;
		goto error;
	}
	_str.size = INTERFACE_JSON_STRING_ALLOC_SIZE;

	field_len = strlen(_field);
	trim_len = field_len;
	if (field_len >= 3 &&
		_field[field_len - 3] == '=' &&
		_field[field_len - 2] == '%')
	{
		const char	_spec = _field[field_len - 1];

		if (_spec == 's')
		{
			format_type = json_tok_str;
			has_format_type = 1;
		}
		else if (_spec == 'd' || _spec == 'i' || _spec == 'u' ||
			_spec == 'x' || _spec == 'p')
		{
			format_type = json_tok_nbr;
			has_format_type = 1;
		}
		if (has_format_type)
			trim_len = field_len - 3;
	}
	else if (field_len >= 4 &&
		_field[field_len - 4] == '=' &&
		_field[field_len - 3] == '%' &&
		_field[field_len - 2] == 'l')
	{
		const char	_spec = _field[field_len - 1];

		if (_spec == 'd' || _spec == 'i' || _spec == 'u' || _spec == 'x')
		{
			format_type = json_tok_nbr;
			has_format_type = 1;
			trim_len = field_len - 4;
		}
	}

	if (has_format_type)
	{
		format_buf = mem_alloc(trim_len + 1);	// TODO: replace this with mem_dup()
		if (unlikely(!format_buf))
		{
			errnum = error_alloc_fail;
			goto error;
		}
		if (trim_len > 0)
			memcpy(format_buf, _field, trim_len);
		format_buf[trim_len] = '\0';
		format = format_buf;
		_type = format_type;
	}

	{
		size_t	_i = 0;

		while (format[_i])
		{
			if (format[_i] == '%')
			{
				if (format[_i + 1] == '%')
				{
					_i += 2;
					continue;
				}
				field_has_format = 1;
				break;
			}
			_i++;
		}
	}

	if (field_has_format)
	{
		errnum = _json_set_fill_format_with_first(format, &_str, _args, _value);
		if (unlikely(errnum != error_none))
			goto error;
		value = va_arg(*_args, void *);
		if (!has_format_type)
			_type = va_arg(*_args, int);
	}
	else
	{
		errnum = _json_fill_format(format, &_str, _args);
		if (unlikely(errnum != error_none))
			goto error;
		if (!has_format_type)
			_type = va_arg(*_args, int);
	}

	errnum = _json_set(_json, _str.content, value, _type);

error:
	mem_free(format_buf);
	mem_free(_str.content);
	return (errnum);
}

int	_json_unset(
	JSON **_json,
	const char *const restrict _field,
	const int _free
)
{
	const int	_nb_fields = _json_tool_count_field(_field);
	t_json		*_target = _json_get_field(*_json, _field, -1);
	t_json		*_prev = _json_get_field(*_json, _field, _nb_fields - 1);

	if (!_target)
		return (error_none);
	else
	{
		if (_nb_fields == 1)
		{
			if (_target == _prev)
				*_json = _target->next;
			else
				_prev->next = _target->next;
		}
		else if (_prev)
			_prev->child = _target->next;
		if (_free)
			_json_free_content(_target);
	}
	return (error_none);
}
