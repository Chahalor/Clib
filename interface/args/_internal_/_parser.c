// Header

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

static inline int	_is_valid_char(
	const char _c
)
{
	return (isalnum(_c));
}

static inline int	_is_opt(
	const char *restrict _s
)
{
	const unsigned char	_c0 = _s[0];
	const unsigned char	_c1 = _s[1];
	const unsigned char	_c2 = _s[2];

	const int			is_short = (_c0 == '-') & _is_valid_char(_c1) & (_c2 == '\0');
	const int			is_long  = (_c0 == '-') & (_c1 == '-') & _is_valid_char(_c2);
	const int			is_delim = (_c0 == '-') & (_c1 == '-') & (_c2 == '\0');

	return ((is_short | is_long) && !is_delim);
}

static inline _t_args_parser	*_get_sub_parser_of(
	const _t_args_parser *const restrict _parser,
	const char *const restrict _s
)
{	// do a cache system that store the last result and last _s
	_t_args_parser	*result = NULL;

	for (_t_args_parser *_this = _parser->sub_parsers;
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
	_t_args_param	*result = NULL;

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

static inline char	*_root_get_next(
	_t_args_parser_root *const restrict _root
)
{
	return (_root->index < _root->argc ?
				_root->argv[_root->index] :
				NULL
		);
}

static inline char	*_root_pop_next(
	_t_args_parser_root *const restrict _root
)
{
	return (_root->index < _root->argc ?
				_root->argv[_root->index++] :
				NULL
		);
}

static int	_add_param(
	_t_args_param *const restrict _this,
	char *_current
)
{
	unsigned int	_nb_alloc;
	char			**_new;
	int				result = error_none;

	if (unlikely(!_current))
	{
		result = args_error_missing_param;
		goto error;
	}
	else if (!_this->values || _this->nb_values + 1 > _this->max_values)
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
		_this->max_values += _nb_alloc;
	}

	_this->values[_this->nb_values++] = _current;

error:
	return (result);
}

static int	_fill_param(
	_t_args_parser_root *const restrict _root,
	_t_args_param *const restrict _param
)
{
	const _t_args_config *const restrict	_config = _args_config_get();
	char									*_current = NULL;
	int										result = error_none;

	for (_t_args_param	*_this = _param;
		_this != NULL && !result;
		_this = _this->next
	)
	{
		_current = _root_pop_next(_root);
		if (unlikely(!_current))
		{
			result = args_error_missing_param;
			_args_config_set_errnum(result);
			goto error;
		}
		else if (_this->args_type & param_args_type_nargs)
		{
			while (_current && _current[0] != '-' && !result)
			{
				result = _add_param(_this, _current);
				_current = _root_pop_next(_root);
			}
		}
		else
		{
			result = _add_param(_this, _current);
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

	for (_t_args_param	*_this = _params;
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
	_t_args_parser *const restrict _context,
	const char *const restrict _current
)
{
	_t_args_option	*_this;
	int				result = error_none;

	_this = _get_opt_of(_context, _current);
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

static inline int	_parse_sub_parser(
	_t_args_parser_root *const restrict _root,
	_t_args_parser *const restrict _context,
	char *_current
)
{
	int	result = error_none;

	result = _parse_args(_root, _context);

	return (result);
}

static inline int	_parse_args(
	_t_args_parser_root *const restrict _root,
	_t_args_parser *const restrict _parser
)
{
	int		result = error_none;
	char	*_current = _root_get_next(_root);

	while (_current && !result)
	{
		if (_is_opt(_current))
			result = _parse_opt(_root, _parser->options, _current);
		else if (_get_sub_parser_of(_parser, _current))
			result = _parse_sub_parser(_root, _parser, _current);
		else
			result = _parse_params(_root, _parser->params);

		_current = _root_get_next(_root);
	}

	return (result);
}

static int	_build_output(
	_t_args_parser_root *const restrict _root,
	t_args_output *const restrict _output
)
{
	// TODO: later
}

/* ----| Public     |----- */

int	_args_parse(
	_t_args_parser *const restrict _root,
	t_args_output *const restrict _output,
	const int _argc,
	const char *_argv[]
)
{
	char			*_current = NULL;
	bool			_opt_disabled = false;
	_t_args_parser	*_context = (_t_args_parser *)_root;
	int				result;

	result = _check_parser(_root);
	if (unlikely(result))
		goto error;

	_root->index = 1;
	_root->argc = _argc;
	_root->argv = _argv;

	_current = _root_pop_next(_root);
	_context = _get_sub_parser_of(_root, _current);
	if (_context)
	{
		result = _parse_sub_parser(_root, _context, _current);
		goto error;
	}

	while (_root->index < _root->argc && !result)
	{
		_t_args_parser	*_tmp_ctxt = get_sub_parser_of(_context, _current);

		if (_opt_disabled)
			result = _parse_params(_root, _context->params);
		else if (!strcmp(_current, "--"))
			_opt_disabled = true;
		else if (_get_opt_of(_root, _current))
			result = _parse_opt(_root, _context, _current);
		else if (_tmp_ctxt)
			_context = _tmp_ctxt;
		else
		{
			result = _add_param(_context, _current);
			// result = _parse_params(_root, _context->params);
		}

		_current = _root_pop_next(_root);
	}

	if (result)
		goto error;
	else
		result = _build_output(_root, _output);

error:
	return (result);
}