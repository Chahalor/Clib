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

__attribute__((visibility("hidden")))
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

	result->buffer = (char *)(result + 1);
	result->length = 0;
	result->capacity = length;

error:
	return (result);
}

__attribute__((visibility("hidden")))
int	_string_append(
	_t_string_ *const target,
	const char *const src,
	const size_t length
)
{
	void			*_new;
	int				result = error_none;

	if (unlikely(target->length + length > target->capacity))
	{
		const size_t	_alloc_size = sizeof(char) * (target->capacity + _internal_get_config_()->string_alloc_size);

		_new = _alloc_size;
		if (unlikely(!_new))
		{
			result = _internal_get_config_()->errnum;
			goto error;
		}

		strcpy(_new, target->buffer);
		_mem_free_(target->buffer);
		target->buffer = _new;
		target->capacity = _alloc_size;
	}

	memcpy(target->buffer + target->length, src, length);
	target->length += length;

error:
	return (result);
}

__attribute__((visibility("hidden")))
void	_string_free_(
	_t_string_ *const target
)
{
	_mem_free_(target->buffer);
	_mem_free_(target);
}