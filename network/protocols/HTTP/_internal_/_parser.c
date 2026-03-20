/**
 * 
*/

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>

	/* Internal */
#include "_HTTP.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static ssize_t	_parse_headers(
	t_http_header_list *const list,
	const char *const raw
)
{
	// parse each header until '\r\n\r\n'
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
 * @note `buf shoudl ahve the rigth size to store the string
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
	int *const error
)
{
	t_http_methods		_method;
	t_http_version		_version;
	ssize_t				i = 0;
	t_http_header_list	_list;
	t_http				*result = NULL;

	_method = _http_str_to_method(raw);
	if (_method == HTTP_ERROR)
	{
		*error = HTTP_ERROR_BAD_REQUEST;
		return (NULL);
	}

	while (raw[i] && (isalpha(raw[i]) || isspace(raw[i])) && raw[i] != '\n')
		i++;

	if (strcmp("HTTP/", &raw[i]))
		i += 5;
	else if (strcmp("HTTPS/", &raw[i]))
		i += 6;
	else
	{
		*error = HTTP_ERROR_BAD_REQUEST;
		return (NULL);
	}

	_version = _http_parse_version(&raw[i]);
	if (unlikely(!_version))
	{
		*error = HTTP_ERROR_BAD_REQUEST;
		return (NULL);
	}
	else if (_version > HTTP_MAX_SUPPORTED_VERSION)
	{
		*error = VERSION_NOT_SUPPORTED;
		return (NULL);
	}

	i = _parse_headers(_list, &raw[i]);
	if (unlikely(i < 0))
	{
		// TODO: handle error
	}
}
