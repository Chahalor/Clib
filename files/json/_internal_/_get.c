// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "_json.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

int	_json_get(
	JSON *const restrict _json,
	const char *const restrict _format,
	va_list *const restrict _args,
	void *restrict *const restrict _result
)
{
	t_json_str	_field = {0};
	int			errnum = error_none;

	_field.content = mem_alloc(sizeof(char) * INTERFACE_JSON_STRING_ALLOC_SIZE);
	if (unlikely(!_field.content))
	{
		errnum = error_alloc_fail;
		goto error;
	}
	_field.size = INTERFACE_JSON_STRING_ALLOC_SIZE;

	errnum = _json_fill_format(_format, &_field, _args);
	if (unlikely(errnum != error_none))
		goto error;

	*_result = _json_access_field(_json, _field.content, -1);

error:
	mem_free(_field.content);
	return (errnum);
}