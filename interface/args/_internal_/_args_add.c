// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "_args.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

t_args_parser	*_args_parser_add_sub(
	t_args_parser *const _parent,
	const char *const _name,
	const char *const _desc
)
{
	t_args_parser	*_last = NULL;
	t_args_parser	*new = NULL;

	new = _args_mem_new_parser(_name, _desc);
	if (unlikely(!new))
		goto error;

	for (t_args_parser	*_this = _parent->sub_parsers;
		_this;
		_this = _this->next
	)
	{
		_last = _this;
	}

	if (!_last)
		_parent->sub_parsers = new;
	else
		_last->next = new;

error:
	return (new);
}

t_args_option	*_args_parser_add_option(
	t_args_parser *const _parent,
	const char *const _long_name,
	const char _sname,
	const char *const _desc
)
{
	char			*_lname = NULL;
	t_args_option	*_last = NULL;
	t_args_option	*new = NULL;

	if (_long_name && _long_name[0] == '-')
	{
		if (_long_name[1] == '-')
			_lname = _long_name + 2;
		else
			_lname = _long_name + 1;
	}

	new = _args_mem_new_option(_lname, _sname, _desc);
	if (unlikely(!new))
		goto error;

	for (t_args_option	*_this = _parent->options;
		_this;
		_this = _this->next
	)
	{
		_last = _this;
	}

	if (!_last)
		_parent->options = new;
	else
		_last->next = new;

error:
	return (new);
}

t_args_param	*_args_parser_add_param(
	t_args_parser *const _parent,
	const char *const _name,
	const char *const _desc,
	const int _spec,
	const int _type
)
{
	t_args_param	*_last = NULL;
	t_args_param	*new = NULL;

	new = _args_mem_new_param(_name, _desc, _spec, _type);
	if (unlikely(!new))
		goto error;

	for (t_args_param	*_this = _parent->params;
		_this;
		_this = _this->next
	)
	{
		_last = _this;
	}

	if (!_last)
		_parent->params = new;
	else
		_last->next = new;

error:
	return (new);
}

t_args_param	*_args_option_add_param(
	t_args_option *const _parent,
	const char *const _name,
	const char *const _desc,
	const int _spec,
	const int _type
)
{
	t_args_param	*_last = NULL;
	t_args_param	*new = NULL;

	new = _args_mem_new_param(_name, _desc, _spec, _type);
	if (unlikely(!new))
		goto error;

	for (t_args_param	*_this = _parent->params;
		_this;
		_this = _this->next
	)
	{
		_last = _this;
	}

	if (!_last)
		_parent->params = new;
	else
		_last->next = new;

error:
	return (new);
}