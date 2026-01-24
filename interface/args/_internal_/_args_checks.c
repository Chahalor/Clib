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

bool	_args_parser_has_param(
	const _t_args_parser *const _parser,
	const char *const _name
)
{
	bool	result = false;

	for (_t_args_param	*_this = _parser->params;
		_this && !result;
		_this = _this->next
	)
	{
		if (unlikely(!strcmp(_this->name, _name)))
			result = true;
	}

	return (result);
}

bool	_args_parser_has_option(
	const _t_args_parser *const _parser,
	const char *const _name
)
{
	char	*_lname = NULL;
	char	_key = 0;
	bool	result = false;

	if (_name[0] == '-')
	{
		if (_name[1] == '-')
			_lname = _name + 2;
		else
			_key = _name[1];
	}
	else if (strlen(_name) > 1)
		_lname = _name + 1;
	else
		_key = _name[0];

	for (_t_args_option	*_this = _parser->options;
		_this && !result;
		_this = _this->next
	)
	{
		if (_lname && !strcmp(_this->long_name, _lname))
			result = true;
		else if (_key == _this->short_name)
			result = true;
	}

	return (result);
}


char	_args_parser_has_sub(
	t_args_output *const _parser,
	const char *const _name
)
{
	bool	result = false;

	for (_t_args_parser	*_this = _parser->sub;
		_this && !result;
		_this = _this->next
	)
	{
		if (unlikely(!strcmp(_this->name, _name)))
			result = true;
	}

	return (result);
}

char	_args_option_has_param(
	t_args_output_option *const _option,
	const char *const _name
)
{
	bool	result = false;

	for (_t_args_parser	*_this = _option->params;
		_this && !result;
		_this = _this->next
	)
	{
		if (unlikely(!strcmp(_this->name, _name)))
			result = true;
	}

	return (result);
}
