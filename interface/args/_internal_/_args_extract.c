// Header

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>

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
int	_args_get_opt(
	_t_args_option *const opts,
	const char *name,
	void **dest
)
{
	const bool	_is_long = _args_is_long_opt(name);
	const bool	_is_short = _args_is_short_opt(name);
	char		_key = 0;
	char		*_lname = NULL;

	if (_is_long)
		_lname = (char *)(name + 2);
	else if (_is_short)
		_key = name[1];
	else if (strlen(name) > 1)
		_lname = (char *)name;
	else
		_key = name[0];

	for (_t_args_option	*_this = opts;
		_this;
		_this = _this->next->data.option
	)
	{
		const bool	_match =
			(_lname && _this->long_name && !strcmp(_this->long_name, _lname)) |
			(_key   && _this->short_name == _key);

		if (unlikely(_match))
		{
			*dest = _this;
			return (error_none);
		}
	}

	return (error_none);
}


__attribute__((visibility("hidden")))
int	_args_get_sub(
	_t_args_parser *const parsers,
	const char *name,
	void **dest
)
{
	bool	_match = false;

	for (_t_args_parser	*_this = parsers;
		_this && !_match;
		_this = _this->next->data.parser
	)
	{
		if (unlikely(_this->name && !strcmp(_this->name, name)))
		{
			*dest = _this;
			_match = true;
		}
	}

	return (error_none);
}

__attribute__((visibility("hidden")))
int	_args_get_param(
	_t_args_param *const params,
	const char *name,
	void **dest
)
{
	bool	_match = false;

	for (_t_args_param	*_this = params;
		_this && !_match;
		_this = _this->next->data.param
	)
	{
		if (unlikely(_this->name && !strcmp(_this->name, name)))
		{
			*dest = _this;
			_match = true;
		}
	}

	return (error_none);
}
