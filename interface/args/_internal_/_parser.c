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
	// TODO
}

static inline int	_check_parser(
	const _t_args_parser *const restrict _parser
)
{
	// TODO
}

static inline char	*_root_get_next(
	_t_args_parser_root *const restrict _root
)
{
	return (_root->index < _root->index ?
				_root->argv[_root->index++] :
				NULL
		);
}

static int	_add_param(
	_t_args_parser_root *const restrict _root,
	_t_args_param *const restrict _this
)
{
	unsigned int	_nb_alloc;
	char			**_new;
	int				result = error_none;

	if (!_this->values || _this->nb_values + 1 > _this->max_values)
	{
		if (_this->args_type & param_args_type_nargs)
			_nb_alloc = _ARGS_PARSE_PARAM_ALLOC_SIZE;
		else
			_nb_alloc = 1;

		_new = mem_alloc(sizeof(char *) * (_this->max_values + _nb_alloc));
		if (unlikely(!_new))
		{
			result = error_alloc_fail;
			_args_config_set_errnum(result);
			goto error;
		}

		for (unsigned int	_i = 0;
			_i < _this->max_values;
			_i++
		)
		{
			_new[_i] = _this->values[_i];
		}

		mem_free(_this->values);
		_this->values = _new;
	}

	_this->values[_this->nb_values++] =_root_get_next(_root);

error:
	return (result);
}

static int	_fill_param(
	_t_args_parser_root *const restrict _root,
	_t_args_param *const restrict _param
)
{
	const _t_args_config *const restrict	_config = _args_config_get();
	int										result = error_none;

	for (_t_args_param	*_this = _param;
		_this != NULL && !result;
		_this = _this->next
	)
	{
		if (_root->index > _root->argc)
		{
			result = args_error_missing_param;
			_args_config_set_errnum(result);
			goto error;
		}
		else if (_this->args_type & param_args_type_nargs)
		{
			while (_root->index < _root->argc && _root->argv[_root->argc][0] != '-' && !result)
				result = _add_param(_root, _this);
		}
	}

error:
	return (result);
}

static int	_parse_params(
	_t_args_parser_root *const restrict _root,
	_t_args_param *const restrict _params
)
{
	int	result = error_none;

	for (_t_args_param *_this = _params;
		_this != NULL && !result;
		_this = _this->next
	)
	{
		result = _fill_param(_root, _this);
	}

error:
	return (result);
}

static int	_parse_opt(
	_t_args_parser_root *const restrict _root,
	_t_args_option *const restrict _opt
)
{
	_t_args_option	*_this;
	int				result = error_none;

	_this = _get_opt_of(_opt, _root->argv[_root->index]);
	if (unlikely(!_this))
	{
		result = args_error_unknown_option;
		_args_config_set_errnum(result);
		goto error;
	}

	_this->nb_calls++;
	if (_this->params)
		result = _parse_params(_root, _this->params);

error:
	return (result);
}

static inline int	_parse_args(
	_t_args_parser_root *const restrict _root,
	_t_args_parser *const restrict _parser
)
{
	int	result;

	while (_root->index < _root->argc && !result)
	{
		if (_is_opt(_root->argv[_root->index]))
			result = _parse_opt(_root, _root->parser->options);
		else if (_is_sub_parser_of(_parser, _root->argv[_root->index]))
			result = _parse_sub_parser(_root, _parser);
		else
			result = _parse_params(_root, _parser->params);
	}
	
}

static inline int	_parse_sub_parser(
	_t_args_parser_root *const restrict _root,
	_t_args_parser *const restrict _parser
)
{
	int			result = error_none;
	const char	*_current = _root_get_next(_root);

	for (_t_args_parser	*_sub = _parser->sub_parsers;
		_sub != NULL && !result;
		_sub = _sub->next
	)
	{
		if (strcmp(_sub->name, _current))
		{
			result = _parse_args(_root, _sub);
			break ;
		}
	}

	return (result);
}

static int	_build_output(
	_t_args_parser_root *const restrict _root,
	t_args_output *const restrict _output
)
{
	// TODO
}

/* ----| Public     |----- */

int	_args_parse(
	_t_args_parser_root *const restrict _root,
	t_args_output *const restrict _output,
	const int _argc,
	const char *_argv[]
)
{
	int	result;

	_root->argc = _argc;
	_root->argv = _argv;

	result = _check_parser(_root);
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
		result = _parse_opt(_root, _root->parser->options);
	}

	if (!result && _root->parser->sub_parsers)
	{
		result = _parse_sub_parser(_root, _root->parser->sub_parsers);
	}
	else
		result = _parse_args(_root, _root->parser->params);

	if (likely(result))
		result = _check_output(_root, _output);

	if (unlikely(result))
		_args_mem_free_parser(_root->parser, true);
	else
		_build_output(_root, _output);

error:
	return (result);
}