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

static int	_json_str_reserve(t_json_str *const restrict _dest, const size_t _add);
static int	_json_str_append_n(t_json_str *const restrict _dest, const char *const restrict _src, const size_t _len);
static int	_json_str_append_char(t_json_str *const restrict _dest, const char _c);
static int	_json_append_uint_base(t_json_str *const restrict _dest, unsigned long long _value, const unsigned int _base);
static int	_json_append_int(t_json_str *const restrict _dest, long long _value);
static int	_add_value(t_json_str *const restrict _dest, const void *const restrict value, const int type);

/* ----| Internals  |----- */

/**
 * @brief	ensure _dest can append _add bytes (plus NUL)
 *
 * @param	_dest	target string buffer
 * @param	_add	number of bytes to append
 *
 * @return	error_none on success, or an error code
 */
static int	_json_str_reserve(
	t_json_str *const restrict _dest,
	const size_t _add
)
{
	size_t	_need;
	size_t	_new_size;
	char	*_new;

	if (unlikely(!_dest))
		return (error_invalid_arg);
	_need = (size_t)_dest->len + _add + 1;
	if (_dest->size >= (int)_need)
		return (error_none);
	_new_size = _dest->size > 0 ? (size_t)_dest->size : INTERFACE_JSON_STRING_ALLOC_SIZE;
	while (_new_size < _need)
		_new_size *= 2;
	_new = mem_alloc(_new_size);
	if (unlikely(!_new))
		return (error_alloc_fail);
	if (_dest->content && _dest->len > 0)
		memcpy(_new, _dest->content, _dest->len);
	mem_free(_dest->content);
	_dest->content = _new;
	_dest->size = (int)_new_size;
	_dest->content[_dest->len] = '\0';
	return (error_none);
}

/**
 * @brief	append a byte sequence to the json string
 *
 * @param	_dest	target string buffer
 * @param	_src	source bytes
 * @param	_len	number of bytes to append
 *
 * @return	error_none on success, or an error code
 */
static int	_json_str_append_n(
	t_json_str *const restrict _dest,
	const char *const restrict _src,
	const size_t _len
)
{
	int	_errnum;

	if (unlikely(!_dest || !_src))
		return (error_invalid_arg);
	_errnum = _json_str_reserve(_dest, _len);
	if (unlikely(_errnum != error_none))
		return (_errnum);
	memcpy(_dest->content + _dest->len, _src, _len);
	_dest->len += (int)_len;
	_dest->content[_dest->len] = '\0';
	return (error_none);
}

/**
 * @brief	append a single character to the json string
 *
 * @param	_dest	target string buffer
 * @param	_c		character to append
 *
 * @return	error_none on success, or an error code
 */
static int	_json_str_append_char(
	t_json_str *const restrict _dest,
	const char _c
)
{
	int	_errnum;

	_errnum = _json_str_reserve(_dest, 1);
	if (unlikely(_errnum != error_none))
		return (_errnum);
	_dest->content[_dest->len++] = _c;
	_dest->content[_dest->len] = '\0';
	return (error_none);
}

/**
 * @brief	append an unsigned integer in base 2..16
 *
 * @param	_dest	target string buffer
 * @param	_value	unsigned value to append
 * @param	_base	numeric base (2..16)
 *
 * @return	error_none on success, or an error code
 */
static int	_json_append_uint_base(
	t_json_str *const restrict _dest,
	unsigned long long _value,
	const unsigned int _base
)
{
	char				_buf[64];
	const char			*digits = "0123456789abcdef";
	int					_len = 0;
	int					_errnum;

	if (unlikely(_base < 2 || _base > 16))
		return (error_invalid_arg);
	if (_value == 0)
		_buf[_len++] = '0';
	while (_value)
	{
		_buf[_len++] = digits[_value % _base];
		_value /= _base;
	}
	for (int i = 0; i < _len / 2; ++i)
	{
		char	tmp = _buf[i];

		_buf[i] = _buf[_len - 1 - i];
		_buf[_len - 1 - i] = tmp;
	}
	_errnum = _json_str_append_n(_dest, _buf, (size_t)_len);
	return (_errnum);
}

/**
 * @brief	append a signed integer in base 10
 *
 * @param	_dest	target string buffer
 * @param	_value	signed value to append
 *
 * @return	error_none on success, or an error code
 */
static int	_json_append_int(
	t_json_str *const restrict _dest,
	long long _value
)
{
	unsigned long long	_uvalue;
	int					_errnum;

	if (_value < 0)
	{
		_errnum = _json_str_append_char(_dest, '-');
		if (unlikely(_errnum != error_none))
			return (_errnum);
		_uvalue = (unsigned long long)(-(_value + 1)) + 1;
	}
	else
		_uvalue = (unsigned long long)_value;
	return (_json_append_uint_base(_dest, _uvalue, 10));
}

/**
 * @brief	append a value based on the _json_format_type
 *
 * @param	_dest	target string buffer
 * @param	value	pointer to the value (or direct string for str)
 * @param	type	_json_format_type selector
 *
 * @return	error_none on success, or an error code
 */
static int	_add_value(
	t_json_str *const restrict _dest,
	const void *const restrict value,
	const int type
)
{
	if (unlikely(!_dest))
		return (error_invalid_arg);
	else if (type == _json_format_type_str)
	{
		const char	*_s = value ? (const char *)value : "null";

		return (_json_str_append_n(_dest, _s, strlen(_s)));
	}
	else if (type == _json_format_type_char)
	{
		const char	_c = value ? *(const char *)value : '\0';

		return (_json_str_append_char(_dest, _c));
	}
	else if (type == _json_format_type_int)
		return (_json_append_int(_dest, (long long)(*(const int *)value)));
	else if (type == _json_format_type_long)
		return (_json_append_int(_dest, *(const long long *)value));
	else if (type == _json_format_type_hex)
		return (_json_append_uint_base(_dest, *(const unsigned long long *)value, 16));
	else
		return (error_invalid_arg);
}

/* ----| Public     |----- */

/**
 * @brief	va remplir le string result en se basant sur le format _str
 *
 * @param	_str	format string
 * @param	_result	output json string buffer
 * @param	_args	va_list with format values
 *
 * @return	error_none on success, or an error code
 */
int	_json_fill_format(
	const char *const restrict _str,
	t_json_str *const restrict _result,
	va_list *const restrict _args
)
{
	size_t	_i = 0;
	int		_errnum = error_none;

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
			_errnum = _json_str_append_char(_result, '%');	// TODO: check if it append the two %
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
				long long	_value = va_arg(*_args, long);

				_errnum = _add_value(_result, &_value, _json_format_type_long);
				if (unlikely(_errnum != error_none))
					return (_errnum);
				_i += 3;
				continue;
			}
			if (_spec == 'x')
			{
				unsigned long long	_value = va_arg(*_args, unsigned long);

				_errnum = _add_value(_result, &_value, _json_format_type_hex);
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
				int	_value = va_arg(*_args, int);

				_errnum = _add_value(_result, &_value, _json_format_type_int);
				break;
			}
			case 's':
			{
				char	*_s = va_arg(*_args, char *);

				_errnum = _add_value(_result, _s, _json_format_type_str);
				break;
			}
			case 'p':
			{
				unsigned long long	_value = (unsigned long long)(uintptr_t)va_arg(*_args, void *);

				_errnum = _json_str_append_n(_result, "0x", 2);
				if (unlikely(_errnum != error_none))
					return (_errnum);
				_errnum = _add_value(_result, &_value, _json_format_type_hex);
				break;
			}
			case 'x':
			{
				unsigned long long	_value = va_arg(*_args, unsigned int);

				_errnum = _add_value(_result, &_value, _json_format_type_hex);
				break;
			}
			case 'c':
			{
				char	_ch = (char)va_arg(*_args, int);

				_errnum = _add_value(_result, &_ch, _json_format_type_char);
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
