// Header

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

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
	_t_string_ *const restrict _result,
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
			_errnum = _string_append_char_(_result, _str[_i]);
			if (unlikely(_errnum != error_none))
				return (_errnum);
			_i++;
			continue;
		}
		if (_str[_i + 1] == '\0')
			return (_string_append_char_(_result, '%'));

		if (_str[_i + 1] == '%')
		{
			_errnum = _string_append_char_(_result, '%');
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

				_errnum = _string_add_value_(_result, &_value, _internal_format_type_long);
				if (unlikely(_errnum != error_none))
					return (_errnum);
				_i += 3;
				continue;
			}
			if (_spec == 'x')
			{
				unsigned long long	_value = va_arg(*_args, unsigned long);

				_errnum = _string_add_value_(_result, &_value, _internal_format_type_hex);
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

				_errnum = _string_add_value_(_result, &_value, _internal_format_type_int);
				break;
			}
			case 's':
			{
				char	*_s = va_arg(*_args, char *);

				_errnum = _string_add_value_(_result, _s, _internal_format_type_str);
				break;
			}
			case 'p':
			{
				unsigned long long	_value = (unsigned long long)(uintptr_t)va_arg(*_args, void *);

				_errnum = _string_append_n_(_result, "0x", 2);
				if (unlikely(_errnum != error_none))
					return (_errnum);
				_errnum = _string_add_value_(_result, &_value, _internal_format_type_hex);
				break;
			}
			case 'x':
			{
				unsigned long long	_value = va_arg(*_args, unsigned int);

				_errnum = _string_add_value_(_result, &_value, _internal_format_type_hex);
				break;
			}
			case 'c':
			{
				char	_ch = (char)va_arg(*_args, int);

				_errnum = _string_add_value_(_result, &_ch, _internal_format_type_char);
				break;
			}
			default:
				_errnum = _string_append_char_(_result, '%');
				if (unlikely(_errnum != error_none))
					return (_errnum);
				_errnum = _string_append_char_(_result, c);
				break;
			}
		}
		if (unlikely(_errnum != error_none))
			return (_errnum);
		_i += 2;
	}
	return (error_none);
}
