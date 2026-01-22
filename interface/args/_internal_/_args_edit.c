// Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdarg.h>

	/* Internal */
#include "_args.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

__attribute__((visibility("hidden")))
int	_args_add_sub_parser(
	_t_args_parser *const parent,
	const char *const name,
	_t_args_parser **const dest
)
{
	_t_args_parser	*_new = NULL;
	_t_args_parser	*_last = NULL;
	int				result = error_none;

	_new = _args_mem_new_parser(name);
	if (unlikely(!_new))
	{
		result = _args_config_get()->errnum;
		goto error;
	}

	for (_t_args_parser	*_this = parent->sub_parsers;
		_this;
		_this = _this->next
	)
	{
		_last = _this;
	}

	if (_last)
		_last->next = _new;
	else
		parent->sub_parsers = _new;

error:
	return (result);
}

__attribute__((visibility("hidden")))
int	_args_add_param_to_parser(
	_t_args_parser *const parent,
	const char *const name,
	_t_args_param **const dest
)
{
	_t_args_param	*_new;
	_t_args_param	*_last;
	int				result;

	_new = _args_mem_new_param(name);
	if (unlikely(!_new))
	{
		result = _args_config_get()->errnum;
		goto error;
	}

	for (_t_args_parser	*_this = parent->params;
		_this;
		_this = _this->next
	)
	{
		_last = _this;
	}

	if (_last)
		_last->next = _new;
	else
		parent->params = _new;

error:
	return (result);
}

__attribute__((visibility("hidden")))
int	_args_add_param_to_option(
	_t_args_option *const parent,
	const char *const name,
	_t_args_param **const dest
)
{
	_t_args_param	*_new;
	_t_args_param	*_last;
	int				result;

	_new = _args_mem_new_param(name);
	if (unlikely(!_new))
	{
		result = _args_config_get()->errnum;
		goto error;
	}

	for (_t_args_parser	*_this = parent->params;
		_this;
		_this = _this->next
	)
	{
		_last = _this;
	}

	if (_last)
		_last->next = _new;
	else
		parent->params = _new;

error:
	return (result);
}

__attribute__((visibility("hidden")))
int	_args_add_option(
	_t_args_parser *const parent,
	const char *const lname,
	const char sname,
	_t_args_option **const dest
)
{
	_t_args_option	*_new;
	_t_args_option	*_last;
	int				result;

	_new = _args_mem_new_option(lname, sname);
	if (unlikely(!_new))
	{
		result = _args_config_get()->errnum;
		goto error;
	}

	for (_t_args_parser	*_this = parent->options;
		_this;
		_this = _this->next
	)
	{
		_last = _this;
	}

	if (_last)
		_last->next = _new;
	else
		parent->options = _new;

error:
	return (result);
}

__attribute__((visibility("hidden")))
int	_args_set_desc_parser(
	_t_args_parser *const target,
	const enum _e_args_set_desc_actions _action,
	const char *const _fmt,
	va_list *const _list
)
{
	char	*_extanded = NULL;
	int		result = error_none;

	if (_action == _args_set_desc_add)
	{
		_extanded = _

	}
	else
	{
		mem_free(target->desc);
	}

	return (result);
}