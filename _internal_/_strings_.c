// Header

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>

	/* Internal */
#include "_config_.h"
#include "_internal_.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

/**
 * @brief	ensure _dest can append _add bytes (plus NUL)
 *
 * @param	_dest	target string buffer
 * @param	_add	number of bytes to append
 *
 * @return	error_none on success, or an error code
 */
int	_string_reserve_(
	_t_string_ *const restrict _dest,
	const size_t _add
)
{
	size_t	_need;
	size_t	_new_size;
	char	*_new;

	if (unlikely(!_dest))
		return (error_invalid_arg);

	_need = (size_t)_dest->len + _add + 1;
	if (_dest->capacity>= (int)_need)
		return (error_none);

	_new_size = _dest->capacity> 0 ? (size_t)_dest->capacity: INTERFACE_JSON_STRING_ALLOC_SIZE;
	while (_new_size < _need)
		_new_size *= 2;
	_new = _mem_alloc_(_new_size);
	if (unlikely(!_new))
		return (error_alloc_fail);

	if (_dest->content && _dest->len > 0)
		memcpy(_new, _dest->content, _dest->len);
	_mem_free_(_dest->content);
	_dest->content = _new;
	_dest->capacity= (int)_new_size;
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
int	_string_append_n_(
	_t_string_ *const restrict _dest,
	const char *const restrict _src,
	const size_t _len
)
{
	int	_errnum;

	if (unlikely(!_dest || !_src))
		return (error_invalid_arg);
	_errnum = _string_reserve_(_dest, _len);
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
int	_string_append_char_(
	_t_string_ *const restrict _dest,
	const char _c
)
{
	int	_errnum;

	_errnum = _string_reserve_(_dest, 1);
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
int	_string_append_uint_base_(
	_t_string_ *const restrict _dest,
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
	_errnum = _string_append_n_(_dest, _buf, (size_t)_len);
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
int	_string_append_int_(
	_t_string_ *const restrict _dest,
	long long _value
)
{
	unsigned long long	_uvalue;
	int					_errnum;

	if (_value < 0)
	{
		_errnum = _string_append_char_(_dest, '-');
		if (unlikely(_errnum != error_none))
			return (_errnum);
		_uvalue = (unsigned long long)(-(_value + 1)) + 1;
	}
	else
		_uvalue = (unsigned long long)_value;
	return (_string_append_uint_base_(_dest, _uvalue, 10));
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
int	_string_add_value_(
	_t_string_ *const restrict _dest,
	const void *const restrict value,
	const int type
)
{
	if (unlikely(!_dest))
		return (error_invalid_arg);
	else if (type == _internal_format_type_str)
	{
		const char	*_s = value ?
							(const char *)value :
							"null";

		return (_string_append_n_(_dest, _s, strlen(_s)));
	}
	else if (type == _internal_format_type_char)
	{
		const char	_c = value ?
							*(const char *)value :
							'\0';

		return (_string_append_char_(_dest, _c));
	}
	else if (type == _internal_format_type_int)
		return (_string_append_int_(_dest, (long long)(*(const int *)value)));
	else if (type == _internal_format_type_long)
		return (_string_append_int_(_dest, *(const long long *)value));
	else if (type == _internal_format_type_hex)
		return (_string_append_uint_base_(_dest, *(const unsigned long long *)value, 16));
	else
		return (error_invalid_arg);
}

// __attribute__((visibility("hidden")))
_t_string_	*_string_new_(
	size_t length
)
{
	_t_string_	*result = NULL;

	if (unlikely(!length))
		length = _internal_get_config_()->string_alloc_size;

	result = _mem_alloc_(sizeof(_t_string_) + (sizeof(char) * length));
	if (unlikely(!result))
		goto error;

	result->content = (char *)(result + 1);
	result->len = 0;
	result->capacity = length;

	memset(result->content, 0, result->capacity);

error:
	return (result);
}

// __attribute__((visibility("hidden")))
_t_string_	*_string_new_form_char_(
	const char *const _src
)
{
	const size_t	_length = _src ?
								strlen(_src) :
								0;
	_t_string_		*result;

	result = _string_new_(_length);
	if (unlikely(!result))
		goto error;

	_string_append_(result, _src, _length);

error:
	return (result);
}

// __attribute__((visibility("hidden")))
int	_string_append_(
	_t_string_ *const target,
	const char *const src,
	const size_t length
)
{
	void			*_new;
	int				result = error_none;

	if (unlikely(target->len + length > target->capacity))
	{
		const size_t	_alloc_size = sizeof(char) * (target->capacity + _internal_get_config_()->string_alloc_size);

		_new = _mem_alloc_(_alloc_size);
		if (unlikely(!_new))
		{
			result = _internal_get_config_()->errnum;
			goto error;
		}

		strcpy(_new, target->content);
		_mem_free_(target->content);
		target->content = _new;
		target->capacity = _alloc_size;
	}

	memcpy(target->content + target->len, src, length);
	target->len += length;
	target->content[target->len] = 0;

error:
	return (result);
}

// __attribute__((visibility("hidden")))
void	_string_free_(
	_t_string_ *const target
)
{
	_mem_free_(target->content);
	_mem_free_(target);
}