/**
 * @file _methods.c
 * @brief Internal HTTP method string conversion helpers.
 */

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>
#include <stdio.h>

	/* Internal */
#include "_HTTP.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

/**
 * @brief	compare `s1` and `s2`, by a max of strlen(`s2`) bytes, the first word of `s1` should be followed by a space of a `\0`
*/
static bool	_cmp(
	const char *const s1,
	const char *const s2
)
{
	size_t	_size = strlen(s2);

	return (
		strncmp(s1, s2, _size) &&
		(isspace(s1[_size] || !s1[_size])
	));
}

/* ----| Public     |----- */

#ifndef _NET_PROT_HTTP_METHODS_NB_STRING
# if HTTP_MAX_SUPPORTED_VERSION == HTTP_1_0
#  define	_NET_PROT_HTTP_METHODS_NB_STRING	3
# elif HTTP_MAX_SUPPORTED_VERSION >= HTTP_1_1
#  define	_NET_PROT_HTTP_METHODS_NB_STRING	8
# else
#  define	_NET_PROT_HTTP_METHODS_NB_STRING	9
# endif
#endif

const char	*_http_methods_str(
	const t_http_methods method
)
{
	const char	*strings[_NET_PROT_HTTP_METHODS_NB_STRING + 1] = {
		# if HTTP_MAX_SUPPORTED_VERSION == HTTP_1_0
		[HTTP_ERROR] = "ERROR", [HTTP_GET] = "GET", [HTTP_POST] = "POST", [HTTP_HEAD] = "HEAD"
		# elif HTTP_MAX_SUPPORTED_VERSION >= HTTP_1_1
		, [HTTP_PUT] = "PUT", [HTTP_DELETE] = "DELETE", [HTTP_OPTIONS] = "OPTIONS", [HTTP_TRACE] = "TRACE", [HTTP_CONNECT] = "CONNECT"
		# else
		, [HTTP_PATCH] = "PATCH"
		# endif
	};

	return (strings[method]);
}

t_http_methods	_http_str_to_method(
	const char *str
)
{
	if (!str)
		return (HTTP_ERROR);

	switch (str[0])
	{
		case 'G':
			if (!strncmp(str, "GET", 3))
				return (HTTP_GET);
			break;

		case 'P':
			if (!strncmp(str, "POST", 4))
				return (HTTP_POST);
#if HTTP_MAX_SUPPORTED_VERSION >= HTTP_VERSION(1, 1)
			if (!strncmp(str, "PUT", 3))
				return (HTTP_PUT);
			if (!strncmp(str, "PATCH", 5))
				return (HTTP_PATCH);
#endif
			break;

		case 'H':
			if (!strncmp(str, "HEAD", 4))
				return (HTTP_HEAD);
			break;

#if HTTP_MAX_SUPPORTED_VERSION >= HTTP_1_1
		case 'D':
			if (!strncmp(str, "DELETE", 6))
				return (HTTP_DELETE);
			break;

		case 'O':
			if (!strncmp(str, "OPTIONS", 7))
				return (HTTP_OPTIONS);
			break;

		case 'T':
			if (!strncmp(str, "TRACE", 5))
				return (HTTP_TRACE);
			break;

		case 'C':
			if (!strncmp(str, "CONNECT", 7))
				return (HTTP_CONNECT);
			break;
#endif
	}

	return (HTTP_ERROR);
}
