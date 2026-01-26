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
#include <stdio.h>

	/* Internal */
#include "_args.h"

	/* External */
		//...

/* ----| Prototypes |----- */

bool _args_output_parser_has_option(
	const t_args_output_parser *const _output,
	const char *const _name
);

/* ----| Internals  |----- */

static inline int	_check_options(
	_t_args_option *const restrict options
)
{
	int	result = error_none;

	for (_t_args_option *_this = options;
		_this != NULL && !result;
		_this = _this->next
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
			_opt = _opt->next
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
		_this = _this->next
	)
	{
		if (unlikely(!_this->name || !_this->name[0] || _this->name[0] == '-'))
			result = args_error_ambiguous_option;
		else if (_this->specs & args_param_specs_nargs && _this->next != NULL)
			result = args_error_conflict;
		else
		{
			for (_t_args_param	*_parm = params;
				_parm != NULL && !result;
				_parm = _parm->next
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

	for (const _t_args_parser	*_this = parser;
		_this != NULL && !result;
		_this = _this->next
	)
	{
		if (unlikely(!_this->name || !_this->name[0] || _this->name[0] == '-'))
			result = args_error_ambiguous_option;
		else
		{
			for (const _t_args_parser	*_psr = parser;
				_psr != NULL && !result;
				_psr = _psr->next
			)
			{
				if (_psr != _this && (!_psr->name || !strcmp(_this->name, _psr->name)))
					result = args_error_duplicate;
			}
			result = _check_options(_this->options);
			result = !result ?
						_check_params(_this->params) :
						result;
			result = !result ?
						_check_parser(_this->sub_parsers) :
						result;
		}
	}

	return (result);
}

static inline int	_check_output_params(
	const _t_args_param *_params,
	const _t_args_output_param *_out
)
{
	int	result = true;

	const _t_args_param	*_param = _params;
	for (const _t_args_output_param	*_this = _out;
		_this && result;
		_this = _this->next
	)
	{
		if (_this->error)
			result = false;
		else if (!_param)
			result = false;
		else if (_args_param_is_requiered(_param) && strcmp(_this->name, _param->name))
			result = false;
		_param = _param->next;
	}

	return (result);
}

static inline const _t_args_option	*_find_option(
	const _t_args_option *const _options,
	const _t_args_output_option *const _out
)
{
	for (const _t_args_option *_opt = _options;
		_opt;
		_opt = _opt->next
	)
	{
		if (_out->long_name && _opt->long_name &&
			!strcmp(_out->long_name, _opt->long_name))
			return (_opt);
		if (_out->short_name && _opt->short_name &&
			_out->short_name == _opt->short_name)
			return (_opt);
	}

	return (NULL);
}

static inline int	_check_output_options(
	const _t_args_option *const _options,
	const _t_args_output_option *_out
)
{
	int	result = true;

	for (const _t_args_output_option	*_this = _out;
		_this && result;
		_this = _this->next
	)
	{
		if (_this->error)
			result = false;
		else if (_this->params)
		{
			const _t_args_option	*_opt = _find_option(_options, _this);

			if (!_opt)
				result = false;
			else
				result = _check_output_params(_opt->params, _this->params);
		}
	}

	return (result);
}

static inline int	_check_output_parser(
	const _t_args_parser *const _parser,
	const _t_args_output_parser *_out
)
{
	const _t_args_parser	*_target = NULL;
	int						result = true;
	
	for (const _t_args_parser	*_this = _parser;
		_this;
		_this = _this->next
	)
	{
		if (!_out || !_out->name)
		{
			_target = _parser;
			break ;
		}
		if (_this->name && !strcmp(_this->name, _out->name))
		{
			_target = _this;
			break ;
		}
	}
	if (!_target)
		_target = _parser;

	if (_args_output_parser_has_option(_out, "help") ||
		_args_output_parser_has_option(_out, "-h"))
		_args_builtin_help(_parser, NULL);

	result = _check_output_options(_target->options, _out->options);
	result = result ?
				result :
				_check_output_params(_target->params, _out->params);

	return (result);
}

/* ----| Public     |----- */

bool	_args_parser_has_param(
	const t_args_output *const _output,
	const char *const _name
)
{
	bool	result = false;

	if (unlikely(!_output || !_output->root))
		return (false);

	for (_t_args_output_param	*_this = _output->root->params;
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
	const t_args_output *const _output,
	const char *const _name
)
{
	char	*_lname = NULL;
	char	_key = 0;
	bool	result = false;

	if (unlikely(!_output || !_output->root))
		return (false);

	if (_name[0] == '-')
	{
		if (_name[1] == '-')
			_lname = (char *)(_name + 2);
		else
			_key = _name[1];
	}
	else if (strlen(_name) > 1)
		_lname = (char *)_name;
	else
		_key = _name[0];

	for (_t_args_output_option	*_this = _output->root->options;
		_this && !result;
		_this = _this->next
	)
	{
		if (_lname && _this->long_name && !strcmp(_this->long_name, _lname))
			result = true;
		else if (_key == _this->short_name)
			result = true;
	}

	return (result);
}


bool _args_output_parser_has_option(
	const t_args_output_parser *const _output,
	const char *const _name
)
{
	char	*_lname = NULL;
	char	_key = 0;
	bool	result = false;

	if (unlikely(!_output || !_output->options))
		return (false);

	if (_name[0] == '-')
	{
		if (_name[1] == '-')
			_lname = (char *)(_name + 2);
		else
			_key = _name[1];
	}
	else if (strlen(_name) > 1)
		_lname = (char *)_name;
	else
		_key = _name[0];

	for (_t_args_output_option	*_this = _output->options;
		_this && !result;
		_this = _this->next
	)
	{
		if (_lname && _this->long_name && !strcmp(_this->long_name, _lname))
			result = true;
		else if (_key == _this->short_name)
			result = true;
	}

	return (result);
}

char	_args_parser_has_sub(
	const t_args_output *const _output,
	const char *const _name
)
{
	bool	result = false;

	if (unlikely(!_output || !_output->root))
		return (false);

	for (_t_args_output_parser	*_this = _output->root->sub;
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
	const t_args_output_option *const _option,
	const char *const _name
)
{
	bool	result = false;

	if (unlikely(!_option))
		return (false);

	for (_t_args_output_param	*_this = _option->params;
		_this && !result;
		_this = _this->next
	)
	{
		if (unlikely(!strcmp(_this->name, _name)))
			result = true;
	}

	return (result);
}

int	_args_check_output(
	const _t_args_root *const _root,
	const _t_args_output *const _output
)
{
	int	result;

	if (unlikely(!_root || !_output || !_output->root))
		result = error_invalid_arg;
	else if (_output->error)
		result = _output->error;
	else if (_output->root->sub)
		result = _check_output_parser(_root->parser->sub_parsers, _output->root->sub);
	else
		result = _check_output_parser(_root->parser, _output->root);

error:
	return (result);
}
