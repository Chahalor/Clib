/**
 * @file _string.c
 * @brief Internal HTTP request serialization helpers.
 */

/* ----| Headers    |----- */
	/* Standard */
#include <stdio.h>

	/* Internal */
#include "_HTTP.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

char	*_http_request_raw(
	const t_http *const http,
	char *const buff,
	size_t size
)
{
	bool	_dynamique = false;
	int		_offset = 0;
	char	*dest;

	if (buff && size)
		dest = buff;
	else if (size)
		dest = _g_net_prot_http_allocator.alloc(size);
	else
	{
		_dynamique = true;
		size = 1024;
		dest = _g_net_prot_http_allocator.alloc(size);
	}

	if (unlikely(!dest))
		return (NULL);

	_offset = snprintf(dest, size, "%s %s HTTP/%d.%d\r\n",
				_http_methods_str(http->method),
				http->path,
				http->version / 100,
				http->version % 100
	);

	if (_offset >= (int)size)
		return (NULL);

	if (http->content_length && !_http_header_find(&http->headers, "content-length"))
	{
		int	nb = snprintf(dest + _offset, size - _offset, "Content-Length: %zu\r\n",
					http->content_length);

		if (nb >= (int)(size - _offset))
			return (NULL);

		_offset += nb;
	}

	for (t_http_header	*_it = http->headers.head;
		_it != NULL;
		_it = _it->order_next)
	{
		int	nb = snprintf(dest + _offset, size - _offset, "%s: %s\r\n",
					_it->key, _it->value);

		if (nb >= (int)(size - _offset))
		{
			if (!_dynamique)
				return (NULL);

			size += 1024;
			dest = _g_net_prot_http_allocator.realloc(dest, size);
			if (unlikely(!dest))
				return (NULL);

			nb = snprintf(dest + _offset, size - _offset, "%s: %s\r\n",
					_it->key, _it->value);
		}

		_offset += nb;
	}

	if (_offset + 2 >= (int)size)
	{
		if (!_dynamique)
			return (NULL);

		size += 1024;
		dest = _g_net_prot_http_allocator.realloc(dest, size);
		if (unlikely(!dest))
			return (NULL);
	}

	dest[_offset++] = '\r';
	dest[_offset++] = '\n';

	/* body */
	if (http->content_length && http->body.content)
	{
		while (_dynamique && _offset + http->content_length >= size)
		{
			size += 1024;
			dest = _g_net_prot_http_allocator.realloc(dest, size);
			if (unlikely(!dest))
				return (NULL);
		}

		if (_offset + http->content_length < (int)size)
		{
			memcpy(dest + _offset,
				http->body.content,
				http->content_length);

			_offset += http->content_length;
		}
		else
			return (NULL);
	}

	dest[_offset] = '\0';
	return (dest);
}
