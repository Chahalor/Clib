/**
 * @file _string.c
 * @brief Internal dynamic string helpers for TOML.
*/
/* ----| Headers    |----- */
	/* Standard */
#include <string.h>

	/* Internal */
#include "_toml.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static int	_toml_str_reserve(
	t_toml_str *const dest,
	const size_t add
)
{
	char	*new_content;
	size_t	new_size;

	if (unlikely(!dest))
		return (error_invalid_arg);

	else if (dest->len + add + 1 <= dest->size)
		return (error_none);

	new_size = dest->size ? dest->size : INTERFACE_JSON_STRING_ALLOC_SIZE;
	while (dest->len + add + 1 > new_size)
		new_size *= 2;

	new_content = setting->alloc(new_size);
	if (unlikely(!new_content))
		return (error_alloc_fail);

	else if (dest->content)
		memcpy(new_content, dest->content, dest->len);

	setting->free(dest->content);
	dest->content = new_content;
	dest->size = new_size;
	dest->content[dest->len] = '\0';
	return (error_none);
}

/* ----| Public     |----- */

int	_toml_str_append_n(
	t_toml_str *const dest,
	const char *const src,
	const size_t len
)
{
	int	errnum;

	if (unlikely(!dest || (!src && len)))
		return (error_invalid_arg);

	errnum = _toml_str_reserve(dest, len);
	if (unlikely(errnum != error_none))
		return (errnum);

	if (len)
		memcpy(dest->content + dest->len, src, len);

	dest->len += len;
	dest->content[dest->len] = '\0';
	return (error_none);
}

int	_toml_str_append_char(
	t_toml_str *const dest,
	const char c
)
{
	return (_toml_str_append_n(dest, &c, 1));
}
