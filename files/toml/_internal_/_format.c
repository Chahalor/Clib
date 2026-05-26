/**
 * @file _format.c
 * @brief Internal format and numeric conversion helpers for TOML.
 */
#include "_toml.h"

#include <stdint.h>
#include <string.h>

static int	_toml_append_uint_base(
	t_toml_str *const dest,
	unsigned long long value,
	const unsigned int base
)
{
	char	buf[32];
	size_t	i;

	if (unlikely(!dest || base < 2 || base > 16))
		return (error_invalid_arg);
	i = sizeof(buf);
	if (!value)
		buf[--i] = '0';
	while (value)
	{
		buf[--i] = "0123456789abcdef"[value % base];
		value /= base;
	}
	return (_toml_str_append_n(dest, buf + i, sizeof(buf) - i));
}

static int	_toml_append_int(
	t_toml_str *const dest,
	long long value
)
{
	if (value < 0)
	{
		const int	errnum = _toml_str_append_char(dest, '-');

		if (unlikely(errnum != error_none))
			return (errnum);
		return (_toml_append_uint_base(dest, -(unsigned long long)value, 10));
	}
	return (_toml_append_uint_base(dest, (unsigned long long)value, 10));
}

int	_toml_fill_format(
	const char *const str,
	t_toml_str *const result,
	va_list *const args
)
{
	size_t	i;
	int		errnum;

	if (unlikely(!str || !result || !args))
		return (error_invalid_arg);
	i = 0;
	while (str[i])
	{
		if (str[i] != '%')
		{
			errnum = _toml_str_append_char(result, str[i++]);
			if (unlikely(errnum != error_none))
				return (errnum);
			continue ;
		}
		if (!str[i + 1])
			return (_toml_str_append_char(result, '%'));
		if (str[i + 1] == '%')
		{
			errnum = _toml_str_append_char(result, '%');
			if (unlikely(errnum != error_none))
				return (errnum);
			i += 2;
			continue ;
		}
		if (str[i + 1] == 'l' && (str[i + 2] == 'd' || str[i + 2] == 'i'))
		{
			errnum = _toml_append_int(result, va_arg(*args, long));
			i += 3;
		}
		else if (str[i + 1] == 'l' && str[i + 2] == 'x')
		{
			errnum = _toml_append_uint_base(result,
					va_arg(*args, unsigned long), 16);
			i += 3;
		}
		else if (str[i + 1] == 'd' || str[i + 1] == 'i')
		{
			errnum = _toml_append_int(result, va_arg(*args, int));
			i += 2;
		}
		else if (str[i + 1] == 's')
		{
			const char	*value = va_arg(*args, char *);

			errnum = _toml_str_append_n(result, value ? value : "(null)",
					strlen(value ? value : "(null)"));
			i += 2;
		}
		else if (str[i + 1] == 'p')
		{
			errnum = _toml_str_append_n(result, "0x", 2);
			if (unlikely(errnum != error_none))
				return (errnum);
			errnum = _toml_append_uint_base(result,
					(unsigned long long)(uintptr_t)va_arg(*args, void *), 16);
			i += 2;
		}
		else if (str[i + 1] == 'x')
		{
			errnum = _toml_append_uint_base(result,
					va_arg(*args, unsigned int), 16);
			i += 2;
		}
		else if (str[i + 1] == 'c')
		{
			const char	c = (char)va_arg(*args, int);

			errnum = _toml_str_append_char(result, c);
			i += 2;
		}
		else
		{
			errnum = _toml_str_append_char(result, '%');
			if (unlikely(errnum != error_none))
				return (errnum);
			errnum = _toml_str_append_char(result, str[i + 1]);
			i += 2;
		}
		if (unlikely(errnum != error_none))
			return (errnum);
	}
	return (error_none);
}

char	*_toml_itoa(
	long long value
)
{
	t_toml_str	result;

	memset(&result, 0, sizeof(result));
	if (unlikely(_toml_append_int(&result, value) != error_none))
	{
		mem_free(result.content);
		return (NULL);
	}
	return (result.content);
}

char	*_toml_uitoa(
	unsigned long long value
)
{
	t_toml_str	result;

	memset(&result, 0, sizeof(result));
	if (unlikely(_toml_append_uint_base(&result, value, 10) != error_none))
	{
		mem_free(result.content);
		return (NULL);
	}
	return (result.content);
}
