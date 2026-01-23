// Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdarg.h>

	/* Internal */
#include "_args.h"

	/* External */
#include "_internal_/_internal_.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

/** */
static int	_change_desc(
	const enum _e_args_set_desc_actions _action,
	_t_string_ *_str,
	const char *const _fmt,
	va_list *_list
)
{
	int			result = error_none;

	if (_action == _args_set_desc_add)
	{
		result = _fill_format_(_fmt, _str, _list);
	}
	else
	{
		mem_free(_str->content);
		_str->content = NULL;
	}

error:
	return (result);
}

/* ----| Public     |----- */

// __attribute__((visibility("hidden")))
int	_args_add_sub_parser(
	_t_args_parser *const parent,
	const char *const name,
	ARGSP **const dest
)
{
	ARGSP			*_new = NULL;
	ARGSP			*_last = NULL;
	int				result = error_none;

	_new = _args_mem_new_parser(name);
	if (unlikely(!_new))
	{
		result = _args_config_get()->errnum;
		goto error;
	}

	for (ARGSP	*_this = parent->sub_parsers;
		_this;
		_this = _this->data.parser->next
	)
	{
		_last = _this;
	}

	if (_last)
		_last->data.parser->next = _new;
	else
		parent->sub_parsers = _new;
	if (dest)
		*dest = _new;

error:
	return (result);
}

// __attribute__((visibility("hidden")))
int	_args_add_param_to_parser(
	_t_args_parser *const parent,
	const char *const name,
	ARGSP **const dest
)
{
	ARGSP			*_new;
	ARGSP			*_last;
	int				result;

	_new = _args_mem_new_param(name);
	if (unlikely(!_new))
	{
		result = _args_config_get()->errnum;
		goto error;
	}

	for (ARGSP	*_this = parent->params;
		_this;
		_this = _this->data.param->next
	)
	{
		_last = _this;
	}

	if (_last)
		_last->data.param->next = _new;
	else
		parent->params = _new;
	if (dest)
		*dest = _new;

error:
	return (result);
}

// __attribute__((visibility("hidden")))
int	_args_add_param_to_option(
	_t_args_option *const parent,
	const char *const name,
	ARGSP **const dest
)
{
	ARGSP			*_new;
	ARGSP			*_last;
	int				result;

	_new = _args_mem_new_param(name);
	if (unlikely(!_new))
	{
		result = _args_config_get()->errnum;
		goto error;
	}

	for (ARGSP	*_this = parent->params;
		_this;
		_this = _this->data.param->next
	)
	{
		_last = _this;
	}

	if (_last)
		_last->data.param->next = _new;
	else
		parent->params = _new;
	if (dest)
		*dest = _new;

error:
	return (result);
}

// __attribute__((visibility("hidden")))
int	_args_add_option(
	_t_args_parser *const parent,
	const char *const lname,
	const char sname,
	ARGSP **const dest
)
{
	ARGSP			*_new;
	ARGSP			*_last;
	int				result;

	_new = _args_mem_new_option(lname, sname);
	if (unlikely(!_new))
	{
		result = _args_config_get()->errnum;
		goto error;
	}

	for (ARGSP	*_this = parent->options;
		_this;
		_this = _this->data.option->next
	)
	{
		_last = _this;
	}

	if (_last)
		_last->data.option->next = _new;
	else
		parent->options = _new;
	if (dest)
		*dest = _new;

error:
	return (result);
}

// __attribute__((visibility("hidden")))
int	_args_set_desc_parser(
	_t_args_parser *const target,
	const enum _e_args_set_desc_actions _action,
	const char *const _fmt,
	va_list *const _list
)
{
	_t_string_	*_str = NULL;
	int			result = error_none;

	_str = _string_new_form_char_(target->desc);
	if (unlikely(!_str))
	{
		result = error_alloc_fail;
		goto error;
	}

	result = _change_desc(_action, _str, _fmt, _list);

	_str->content = NULL;
	_string_free_(_str);
error:
	return (result);
}

// __attribute__((visibility("hidden")))
int	_args_set_desc_option(
	_t_args_option *const target,
	const enum _e_args_set_desc_actions _action,
	const char *const _fmt,
	va_list *const _list
)
{
	_t_string_	*_str = NULL;
	int			result = error_none;

	_str = _string_new_form_char_(target->desc);
	if (unlikely(!_str))
	{
		result = error_alloc_fail;
		goto error;
	}

	result = _change_desc(_action, _str, _fmt, _list);

	_str->content = NULL;
	_string_free_(_str);
error:
	return (result);
}

// __attribute__((visibility("hidden")))
int	_args_set_desc_param(
	_t_args_param *const target,
	const enum _e_args_set_desc_actions _action,
	const char *const _fmt,
	va_list *const _list
)
{
	_t_string_	*_str = NULL;
	int			result = error_none;

	_str = _string_new_form_char_(target->desc);
	if (unlikely(!_str))
	{
		result = error_alloc_fail;
		goto error;
	}

	result = _change_desc(_action, _str, _fmt, _list);

	_str->content = NULL;
	_string_free_(_str);
error:
	return (result);
}
