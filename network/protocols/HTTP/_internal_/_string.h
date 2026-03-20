/**
 * 
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

/**
 * allocate the result if `buff` == NULL or `size` == 0
*/
char	*_http_request_raw(
	const t_http *const http,
	char *const buff,
	size_t size
)
{
	char	*dest;
	int		_offset;

	if (buff && size)
		dest = buff;
	else if (size)
		dest = _g_net_prot_http_allocator.alloc(sizeof(char) * size);
	else
		// dynamique alloc logic

	_offset = snprintf(dest, size, "%s %s HTTP/%d.%d\r\n",
						_http_methods_str(http->method), http->path, http->version / 100, http->version - 100);
	
}
