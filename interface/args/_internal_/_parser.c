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

static inline int	_is_opt(
	const char *restrict _s
)
{
	const unsigned char	_c0 = _s[0];
	const unsigned char	_c1 = _s[1];
	const unsigned char	_c2 = _s[2];

	const int			is_short = (_c0 == '-') & (_c1 != '\0') & (_c2 == '\0');
	const int			is_long  = (_c0 == '-') & (_c1 == '-') & (_c2 != '\0');

	return (is_short | is_long);
}

static inline _t_args_parser	*_get_sub_parser_of(
	const _t_args_parser *const restrict _parser,
	const char *const restrict _s
)
{
	_t_args_parser	*result = NULL;

	for (_t_args_parser *_this = _parser->next;
		_this != NULL && !result;
		_this = _this->next
	)
	{
		result = _this->name && !strcmp(_this->name, _s) ?
					_this :
					NULL;
	}

	return (result);
}

static inline _t_args_option	*_get_opt_of(
	const _t_args_parser *const restrict _opts,
	const char *const restrict _s
)
{
	const bool				_is_long = _s[1] != '\0';
	_t_args_option			*result = NULL;

	for (_t_args_option *_this = _opts->next;
		_this != NULL && !result;
		_this = _this->next
	)
	{
		// TODO: check if _s is an option of the _opts context
	}

	return (result);
}

static inline _t_args_param	*_get_param_of(
	const _t_args_param *const restrict _param,
	const char *const restrict _s
)
{
	_t_args_param	*result = false;

	for (_t_args_param *_this = _param->next;
		_this != NULL && !result;
		_this = _this->next
	)
	{
		result = _this->name && !strcmp(_this->name, _s) ?
					_this :
					NULL;
	}

	return (result);
}

static inline int	_check_output(
	const _t_args_parser *const restrict _parser,
	t_args_output *const restrict _output
)
{

}

static inline int	_check_parser(
	const _t_args_parser *const restrict _parser
)
{

}

static int	_add_param(
	_t_args_param *const restrict _this,
)
{
	// TODO: add a new value the _this and realloc the buffer if needed
}

static int	_fill_param(
	_t_args_param *const restrict _param,
	const int _argc,
	const char *_argv[],
	const int _start
)
{
	const _t_args_config *const restrict	_config = _args_config_get();
	int										_i = _start;
	int										result = error_none;

	for (_t_args_param	*_this = _param;
		_this != NULL && !result;
		_this = _this->next
	)
	{
		if (_i > _argc)
		{
			result = args_error_missing_param;
			_args_config_set_errnum(result);
			goto error;
		}
		else if (_this->args_type & param_args_type_nargs)
		{
			while (_i < _argc && _argv[_i][0] != '-')
		}
	}

error:
	return (result);
}

static int	_parse_params(
	_t_args_param *const restrict _opt,
	const int _argc,
	const char *_argv[],
	const int _start
)
{
	int	result = error_none;
	int	_i = _start;

	for (_t_args_param *_param = _opt;
			_param != NULL && !result;
			_param = _param->next
		)
	{
		_i += _fill_param(_param, _argc, _argv, _start + _i);
	}

error:
	return (result);
}

static int	_parse_opt(
	_t_args_parser *const restrict _parser,
	t_args_output *const restrict _output,
	const int _argc,
	const char *_argv[],
	const int _start
)
{
	_t_args_option	*_this;
	int				result = error_none;

	_this = _get_opt_of(_parser, _argv[_start]);
	if (unlikely(!_this))
	{
		result = args_error_unknown_option;
		_args_config_set_errnum(result);
		goto error;
	}

	_this->nb_calls++;
	if (_this->params)
		result = _parse_opt_params(_this, _argc, _argv, _start + 1);

error:
	return (result);
}

static inline int	_parse_args(
	_t_args_parser *const restrict _parser,
	t_args_output *const restrict _output,
	const int _argc,
	const char *_argv[],
	const int _start
)
{
	int	result;
	int	_i;

	_i = _start;
	while (_i < _argc && !result)
	{
		if (_is_opt(_argv[_i]))
			result = _parse_opt(_parser, _output, _argc, _argv, _i);
		else if (_is_sub_parser_of(_parser, _argv[_i]))
			result = _parse_sub_parser(_parser, _output, _argc, _argv, _i);
	}
	
}

static inline int	_parse_sub_parser(
	_t_args_parser *const restrict _parser,
	t_args_output *const restrict _output,
	const int _argc,
	const char *_argv[],
	const int _start
)
{
	
}

/* ----| Public     |----- */

int	_args_parse(
	_t_args_parser *const restrict _parser,
	t_args_output *const restrict _output,
	const int _argc,
	const char *_argv[]
)
{
	int	result;

	result = _check_parser(_parser);
	if (unlikely(result))
		goto error;
	memset(_output, 0, sizeof(t_args_output));

	if (unlikely(_argc == 1))
	{
		// TODO: no args handling
		// result = args_error_invalid_args;
		goto error;
	}
	else if (_is_opt(_argv[1]))
	{
		result = _parse_opt(_parser, _output, _argc, _argv, 1);
	}

	if (!result && _parser->sub_parsers)
	{
		result = _parse_sub_parser(_parser, _output, _argc, _argv, 1);
	}
	else
		result = _parse_args(_parser, _output, _argc, _argv, 1);

	if (likely(result))
		result = _check_output(_parser, _output);

error:
	return (result);
}