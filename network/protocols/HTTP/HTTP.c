/**
 * 
*/

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>
#include <stdlib.h>

	/* Internal */
#include "_internal_/_HTTP.h"
#include "../HTTP.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

t_http	*http_new(void)
{
	return (_http_new(1.0f, HTTP_ERROR, NULL, NULL));
}

/**
 * @brief	will choose automaticly the rigth version parsing based on the request header
 */
t_http	*http_parse_str(
	const char *s
)
{
	int	_err = error_none;

	if (unlikely(!s))
		return (NULL);
	else
		return (_http_parse_1_0(s, strlen(s), &_err));
}

t_http	*http_dup(
	const t_http *const first
)
{
	if (unlikely(!first))
		return (NULL);
	else
		return (_http_dup(first));
}

t_http_header_list	*http_new_list(void)
{
	t_http_header_list	*result;

	result = _g_net_prot_http_allocator.alloc(sizeof(t_http_header_list));
	if(unlikely(!result))
	{
		g_net_prot_http_settings.errno = error_alloc_fail;
		return (NULL);
	}

	_http_setup_header_list(result, 16);

	return (result);
}

t_http_header_list	*http_new_list_size_t(
	const size_t size
)
{
	t_http_header_list	*result;

	result = _g_net_prot_http_allocator.alloc(sizeof(t_http_header_list));
	if(unlikely(!result))
	{
		g_net_prot_http_settings.errno = error_alloc_fail;
		return (NULL);
	}

	_http_setup_header_list(result, size);

	return (result);
}

int	http_add_header(
	t_http_header_list *const list,
	t_http_header *const header
)
{
	return (_http_add_header(list, header));
}

int	http_add_header_raw(
	t_http_header_list *const list,
	const char *const key,
	const char *const value
)
{
	t_http_header	*_new;

	_new = _http_new_header(key, value);
	if (unlikely(!_new))
		return (g_net_prot_http_settings.errno);

	return (http_add_header(list, _new));
}

/**
 * if `buffer` == NULL --> allocate a new char *
 * else                --> write the raw request to the `buffer`
*/
char	*http_to_raw(
	const t_http *const http,
	char *const buffer,
	const size_t size
)
{
	return (_http_request_raw(http, buffer, size));
}

// - Free
void	http_free(
	t_http *const target
)
{
	_http_free(target, false);
}

void				http_free_header_list(
	t_http_header_list *const list
)
{
	_http_free_list(list, true);
}

void	http_free_header(
	t_http_header *const header
)
{
	_http_free_header(header, false);
}
