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
	const _t_args_param *const restrict params
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
		else if (_this->args_type & param_args_type_nargs && _this->next != NULL)
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
	const _t_args_parser *const restrict parser
)
{
	int	result = error_none;

	for (_t_args_parser	*_this = parser;
		_this != NULL && !result;
		_this = _this->next
	)
	{
		if (unlikely(!_this->name || !_this->name[0] || _this->name[0] == '-'))
			result = args_error_ambiguous_option;
		else
		{
			for (_t_args_parser	*_psr = parser;
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

/* ----| Public     |----- */

int	_args_check_output(
	const _t_args_parser *const restrict _parser,
	t_args_output *const restrict _output
)
{
	// TODO
}

int	_args_check_parser(
	const _t_args_parser *const restrict _parser
)
{
	if (unlikely(!_parser->name))
		return (error_invalid_arg);
	else
		return (_check_parser(_parser));
}