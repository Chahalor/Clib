// Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdint.h>
#include <string.h>

	/* Internal */
#include "_json_edit.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |---- */

int	_json_add_child(
	t_json **_target,
	t_json *_child
)
{
	t_json	*_current;

	if (unlikely(!_target))
		return (error_invalid_arg);
	else if (unlikely(!(*_target)))
	{
		*_target = _child;
		return (error_none);
	}
	else if (!(*_target)->child)
		(*_target)->child = _child;
	else
	{
		_current = (*_target)->child;
		while (_current && _current->next)
			_current = _current->next;
		_current->next = _child;
	}
	return (error_none);
}

int	_json_add_next(
	t_json **_target,
	t_json *_next
)
{
	t_json	*_current;

	if (unlikely(!_target))
		return (error_invalid_arg);
	else if (unlikely(!(*_target)))
	{
		*_target = _next;
		return (error_none);
	}
	else if (!(*_target)->next)
		(*_target)->next = _next;
	else
	{
		_current = (*_target)->next;
		while (_current && _current->next)
			_current = _current->next;
		_current->next = _next;
	}
	return (error_none);
}