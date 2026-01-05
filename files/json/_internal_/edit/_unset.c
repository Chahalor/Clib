// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "_json_edit.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static inline char	*_json_unset_va(
	va_list *const restrict _args,
	const char *const restrict _field
)
{
	t_json_str	_str = {0};

	_json_fill_format(_field, &_str, _args);

	return (_str.content);
}

/* ----| Public     |----- */


int	_json_unset(
	JSON **_json,
	const char *restrict _field,
	const int _free,
	va_list *const restrict _args
)
{
	char		*_field_buf = NULL;
	int			_nb_fields = 0;
	t_json		*_target = NULL;
	t_json		*_prev = NULL;
	int			result = error_none;

	if (_args)
	{
		_field_buf = _json_unset_va(_args, _field);
		if (unlikely(!_field_buf))
		{
			result = error_alloc_fail;
			goto cleannup;
		}
		_field = _field_buf;
	}

	_nb_fields = _json_tool_count_field(_field);
	_target = _json_get_field(*_json, _field, -1);
	_prev = _json_get_field(*_json, _field, _nb_fields - 1);

	if (!_target)
	{
		result = error_none;
		goto cleannup;
	}
	else
	{
		t_json	*const parent = (_nb_fields == 1) ? *_json : _prev;
		t_json	*cursor = NULL;
		t_json	*prev_sibling = NULL;

		if (parent)
			cursor = parent->child;
		while (cursor && cursor != _target)
		{
			prev_sibling = cursor;
			cursor = cursor->next;
		}
		if (cursor == _target)
		{
			if (prev_sibling)
				prev_sibling->next = _target->next;
			else if (parent)
				parent->child = _target->next;
		}
		if (_free)
			_json_free_content(_target);
	}

cleannup:
	mem_free(_field_buf);
	return (error_none);
}
