/**
 * @file _parser.c
 * @brief Internal HTTP request parsing helpers.
 */
/* Exemple:
POST /users HTTP/1.1
Host: example.com
Content-Type: application/x-www-form-urlencoded
Content-Length: 49

name=FirstName+LastName&email=bsmth%40example.com
*/

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

	/* Internal */
#include "_HTTP.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

#define _internal_server_error(ret)\
{  \
	g_net_prot_http_settings.errno = error_alloc_fail; \
	*error = INTERNAL_SERVER_ERROR;                    \
	_http_free_list(&_list, true);                     \
	return (ret);                                     \
}

#define _bad_request(ret)\
{  \
	*error = BAD_REQUEST;          \
	_http_free_list(&_list, true); \
	return (ret);                  \
}

int	_next_line(
	const char *const buf,
	const size_t len,
	size_t *_offset,
	const char **_line,
	size_t *const _line_len
)
{
	size_t i = *_offset;

	while (i + 1 < len)
	{
		if (buf[i] == '\r' && buf[i + 1] == '\n')
		{
			*_line = buf + *_offset;
			*_line_len = i - *_offset;
			*_offset = i + 2;
			return (1);
		}
		i++;
	}
	return (0);
}

ssize_t	_str_to_nb(
	const char *const s,
	const char **end
)
{
	int		_sign = 1;
	size_t	i = 0;
	ssize_t	result = 0;

	if (s[0] == '-')
	{
		_sign = -1;
		i++;
	}
	else if (s[0] == '+')
		i++;

	while (isdigit(s[i]))
	{
		result = result * 10 + (s[i] - '0');
		i++;
	}

	result *= _sign;
	*end = &s[i];

	return (result);
}

static ssize_t	_parse_headers(
	t_http_header_list *const list,
	const char *const raw
)
{
	const char	*_line = NULL;
	size_t		_len = strlen(raw);
	size_t		_offset = 0;
	size_t		_line_len = 0;
	ssize_t		_total = 0;

	int	_err = _http_setup_header_list(list, 127);
	if (unlikely(_err))
		return (-1);

	while (_next_line(raw, _len, &_offset, &_line, &_line_len))
	{
		const char		*_sep;
		const char		*_val;
		size_t			_key_len;
		size_t			_val_len;
		t_http_header	*_header;

		if (_line_len == 0)
			return (_offset);

		_sep = memchr(_line, ':', _line_len);
		if (unlikely(!_sep))
			return (-1);

		_key_len = _sep - _line;
		_val = _sep + 1;

		while ((_val < _line + _line_len) && (*_val == ' ' || *_val == '\t'))
			_val++;

		_val_len = (_line + _line_len) - _val;

		char	key_buf[_key_len + 1];
		char	_val_buf[_val_len + 1];

		memcpy(key_buf, _line, _key_len);
		key_buf[_key_len] = '\0';

		memcpy(_val_buf, _val, _val_len);
		_val_buf[_val_len] = '\0';

		_header = _http_new_header(key_buf, _val_buf);
		if (unlikely(!_header))
			return (-1);

		if (unlikely(_http_add_header(list, _header)))
			return (-1);

		_total++;
	}

	return (-1);
}

/* ----| Public     |----- */

t_http_version	_http_parse_version(
	const char *str
)
{
	int	major = 0;
	int	minor = 0;

	if (!str)
		return (0);

	if (str[0] != 'H' || str[1] != 'T' || str[2] != 'T' ||
		str[3] != 'P' || str[4] != '/')
		return (0);

	str += 5;

	if (unlikely(!isdigit(*str)))
		return (0);

	while (isdigit(*str))
	{
		major = major * 10 + (*str - '0');
		str++;
	}

	if (unlikely(*str != '.'))
		return (0);
	str++;

	if (unlikely(!isdigit(*str)))
		return (0);

	while (isdigit(*str))
	{
		minor = minor * 10 + (*str - '0');
		str++;
	}

	if (unlikely(*str != '\0' && (*str != '\r' && *(str + 1) != '\n')))
		return (0);

	return (HTTP_VERSION(major, minor));
}

/**
 * @note `buf` should have the rigth size to store the string
*/
char	*_http_version_to_str(
	t_http_version v,
	char *const buf
)
{
	const int	major = v / 100;
	const int	minor = v % 100;

	if (unlikely(!buf))
		return (NULL);

	buf[0] = 'H';
	buf[1] = 'T';
	buf[2] = 'T';
	buf[3] = 'P';
	buf[4] = '/';

	buf[5] = '0' + major;
	buf[6] = '.';
	buf[7] = '0' + minor;
	buf[8] = '\0';

	return (buf);
}

t_http	*_http_parse_1_0(
	const char *const raw,
	const ssize_t size,
	int *const error
)
{
	t_http_methods		_method;
	t_http_version		_version;
	char				*_path = NULL;
	size_t				_content_length = 0;
	ssize_t				i = 0;
	t_http_header_list	_list = {0};
	t_http_body			_body = {0};
	t_http				*result = NULL;

	_method = _http_str_to_method(raw);
	if (_method == HTTP_ERROR)
		_bad_request(NULL);

	while (i < size && isalpha(raw[i]))
		i++;
	while (i < size && isspace(raw[i]))
		i++;

	/* --- PATH --- */
	ssize_t	start = i;
	while (i < size && raw[i] != ' ')
		i++;

	if (i >= size)
		_bad_request(NULL);

	ssize_t	path_len = i - start;

	_path = _g_net_prot_http_allocator.alloc(sizeof(char) * (path_len + 1));
	if (unlikely(!_path))
	{
		g_net_prot_http_settings.errno = error_alloc_fail;
		*error = INTERNAL_SERVER_ERROR;
		return (NULL);
	}

	memcpy(_path, &raw[start], path_len);
	_path[path_len] = '\0';

	i++;	// skip space before version

	ssize_t	version_start = i;
	while (i < size && raw[i] != '\r')
		i++;

	if (i + 1 >= size || raw[i] != '\r' || raw[i + 1] != '\n')
		_bad_request(NULL)

	char	version_buf[16];
	ssize_t	version_len = i - version_start;

	if (version_len <= 0 || version_len >= (ssize_t)sizeof(version_buf))
		_bad_request(NULL);

	memcpy(version_buf, &raw[version_start], version_len);
	version_buf[version_len] = '\0';

	_version = _http_parse_version(version_buf);
	if (unlikely(!_version))
		_bad_request(NULL)
	else if (_version > HTTP_MAX_SUPPORTED_VERSION)
	{
		*error = VERSION_NOT_SUPPORTED;
		return (NULL);
	}

	i += 2;	// skip \r\n

	ssize_t	header_offset = _parse_headers(&_list, &raw[i]);
	if (unlikely(header_offset < 0))
		_bad_request(NULL);

	i += header_offset;

	t_http_header	*_content = _http_header_find(&_list, "content-length");
	if (_content)
	{
		char	*_end;
		ssize_t	_length;

		_length = _str_to_nb(_content->value, &_end);
		if (_length < 0 || *_end != '\0')
			_bad_request(NULL);

		_content_length = (size_t)_length;

		if (i + (ssize_t)_content_length > size)
			_bad_request(NULL);

		if (_content_length)
		{
			_body.capacity = _content_length + 1;
			_body.content = _g_net_prot_http_allocator.alloc(_body.capacity);
			if (unlikely(!_body.content))
				_internal_server_error(NULL)

			memcpy(_body.content, &raw[i], _content_length);
			_body.content[_content_length] = '\0';
		}
	}

	result = _http_new(_version, _method, _list, _body);
	if (unlikely(!result))
		_internal_server_error(NULL);

	return (result);
}
