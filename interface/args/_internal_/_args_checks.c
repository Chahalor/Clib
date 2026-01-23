// Header

/**
 * name logic:	_module_action_target_sub-action_scope
 * example: 	_args_check_sub_exist_parser
 * 	- module: args
 * 	- action: check somethings
 * 	- target: sub-module, only check sub-modules
 * 	- sub-action: check the existance of
 * 	- scope: parser, from the struct parser
 */

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>
#include <ctype.h>

	/* Internal */
#include "_args.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static inline int	_check_options(
	_t_args_option *const restrict options
)
{
	int	result = error_none;

	for (_t_args_option *_this = options;
		_this != NULL && !result;
		_this = _this->next->data.option
	)
	{
		char	*_name1 = _this->long_name;

		if (unlikely(!_name1 && !_this->short_name))
		{
			result = args_error_ambiguous_option;
			break ;
		}
		else if (!strncmp(_name1, "--", 2))
			_name1 = _name1 + 2;

		for (_t_args_option *_opt = options;
			_opt != NULL && !result;
			_opt = _opt->next->data.option
		)
		{
			char	*_name2 = _opt->long_name;

			if (unlikely(_opt == _this))
				continue ;
			else if (!strncmp(_name2, "--", 2))
				_name2 = _name2 + 2;
			else if (unlikely((_name2 && !strcmp(_name2, _name1) ||
				_opt->short_name == _this->short_name && _opt->short_name != '\0')
			))
				result = args_error_duplicate;
		}
	}

	return (result);
}

static inline int	_check_params(
	_t_args_param *const params
)
{
	int	result = error_none;

	for (_t_args_param	*_this = params;
		_this != NULL && !result;
		_this = _this->next->data.param
	)
	{
		if (unlikely(!_this->name || !_this->name[0] || _this->name[0] == '-'))
			result = args_error_ambiguous_option;
		else if (_this->args_type & param_args_type_nargs && _this->next != NULL)
			result = args_error_conflict;
		else
		{
			for (_t_args_param	*_parm = params;
				_parm != NULL && !result;
				_parm = _parm->next->data.param
			)
			{
				if (_parm != _this && (!_parm->name || !strcmp(_parm->name, _this->name)))
					result = args_error_duplicate;
			}
		}
	}

	return (result);
}

static int	_check_parser(
	const _t_args_parser *const parser
)
{
	int	result = error_none;

	for (_t_args_parser	*_this = (_t_args_parser *)parser;
		_this != NULL && !result;
		_this = _this->next->data.parser
	)
	{
		if (unlikely(!_this->name || !_this->name[0] || _this->name[0] == '-'))
			result = args_error_ambiguous_option;
		else
		{
			for (const _t_args_parser	*_psr = parser;
				_psr != NULL && !result;
				_psr = _psr->next->data.parser
			)
			{
				if (_psr != _this && (!_psr->name || !strcmp(_this->name, _psr->name)))
					result = args_error_duplicate;
			}
			result = _check_options(_this->options->data.option);
			result = !result ?
						_check_params(_this->params->data.param) :
						result;
			result = !result ?
						_check_parser(_this->sub_parsers->data.parser) :
						result;
		}
	}

	return (result);
}

/* ----| Public     |----- */

__attribute__((visibility("hidden")))
bool	_args_check_sub_exist_parser(
	const _t_args_parser *const restrict parser,
	const char *const name
)
{
	int	result = false;

	for (_t_args_parser	*_this = parser->sub_parsers->data.parser;
		_this != NULL && !result;
		_this = _this->next->data.parser
	)
	{
		if (_this->name && !strcmp(_this->name, name))
			result = true;
	}

	return (result);
}

__attribute__((visibility("hidden")))
bool	_args_check_opt_exist_parser(
	const _t_args_parser *const restrict parser,
	const char *name
)
{
	const bool	_is_long =	_args_is_long_opt(name);
	int			result = false;

	if (_is_long)
	{
		if (name[0] == '-')
			name = name + 2;
		for (_t_args_option	*_this = parser->options->data.option;
			_this != NULL && !result;
			_this = _this->next->data.option
		)
		{
			if (_this->long_name && !strcmp(_this->long_name, name))
				result = true;
		}
	}
	else
	{
		if (name[0] == '-')
			name = name + 1;
		for (_t_args_option	*_this = parser->options->data.option;
			_this != NULL && !result;
			_this = _this->next->data.option
		)
		{
			if (_this->short_name && _this->short_name == name[0])
				result = true;
		}
	}

	return (result);
}

__attribute__((visibility("hidden")))
bool	_args_check_param_exist_parser(
	const _t_args_parser *const restrict parser,
	const char *name
)
{
	int	result = false;

	if (name[0] != '-')
	{
		for (_t_args_param	*_this = parser->params->data.param;
			_this != NULL && !result;
			_this = _this->next->data.param
		)
		{
			if (unlikely(!strcmp(_this->name, name)))
				result = true;
		}
	}

	return (result);
}

__attribute__((visibility("hidden")))
int	_args_check_parser(
	const _t_args_parser *const _parser
)
{
	if (unlikely(!_parser->name))
		return (error_invalid_arg);
	else
		return (_check_parser(_parser));
}
