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

static _t_args_parser	*_get_sub_parser_of(
	const _t_args_parser *const restrict _context,
	const char *const restrict _s
)
{
	_t_args_parser			*result = NULL;

	for (_t_args_parser *_this = _context->sub_parsers;
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

static _t_args_option	*_get_opt_of(
	const _t_args_parser *const restrict _context,
	const char *const restrict _s
)
{
	const bool		_is_long = _s && _is_long_opt(_s);
	_t_args_option	*result = NULL;

	for (_t_args_option	*_this = _context->options;
		_this != NULL && !result;
		_this = _this->next
	)
	{
		if (_s && _this->long_name && _is_long && !strcmp(_this->long_name, _s + 2))
			result = _this;
		else if (_s[1] == _this->short_name)
			result = _this;
	}

	return (result);
}

static inline char	*_root_get_next(
	_t_args_parser *const restrict _root
)
{
	return (_root->index < _root->argc ?
				_root->argv[_root->index] :
				NULL
		);
}

static inline char	*_root_pop_next(
	_t_args_parser *const restrict _root
)
{
	return (_root->index < _root->argc ?
				_root->argv[_root->index++] :
				NULL
		);
}

static inline void	_root_push_back(
	_t_args_parser *const restrict _root
)
{
	_root->index -= _root->index > 0 ?
						1 :
						0;
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

static inline int	_parse_context_parser(
	_t_args_parser **_context,
	_t_args_param **pos_cursor,
	_t_args_param **opt_param,
	int *const restrict context_type,
	const bool opt_disabled,
	char *current
)
{
	_t_args_option	*opt = NULL;
	_t_args_parser	*sub = NULL;
	int				result = error_none;

	/* Sub-parser switch (only meaningful in parser context) */
	sub = _get_sub_parser_of(_context, current);
	if (sub)
	{
		*_context = sub;
		*pos_cursor = (*_context)->params;

		/* Usually -- applies globally, so we DO NOT reset opt_disabled.
			If you want it per-context, set opt_disabled=false here. */
		return (result);
	}

	/* Option handling (disabled after "--") */
	if (!opt_disabled && _is_opt(current))
	{
		opt = _get_opt_of(*_context, current);
		if (unlikely(!opt))
		{
			result = args_error_unknown_option;
			_args_config_set_errnum(result);
			return (result);
		}

		/* Optional: enforce repeatability here if you have a flag */
		opt->nb_calls++;
		/* if (!(opt->flags & OPT_REPEATABLE) && opt->nb_calls > 1) ... */

		*opt_param = opt->params;
		if (*opt_param)
			*context_type = args_context_opt;
		return (result);
	}

	/* Positional parameter */
	if (unlikely(!*pos_cursor))
	{
		result = args_error_extra_param;
		_args_config_set_errnum(result);
		return (result);
	}

	/* If this positional is nargs: consume this token into it and keep cursor */
	if ((*pos_cursor)->args_type & param_args_type_nargs)
	{
		/* For positionals nargs, we typically consume everything remaining
			(including things that look like options) once opt_disabled is true.
			If you want "stop at next option" behavior for positionals nargs,
			add a boundary check here. */
		// TODO: consume all other token if opt_disabled == true
		result = _add_param(*pos_cursor, current);
		if (unlikely(result))
		{
			_args_config_set_errnum(result);
			return (result);
		}
		/* stay on same pos_cursor */
	}
	else
	{
		result = _add_param(pos_cursor, current);
		if (unlikely(result))
		{
			_args_config_set_errnum(result);
			return (result);
		}
		pos_cursor = (*pos_cursor)->next;
	}

	return (result);
}

static inline int	_parse_context_opt(
	_t_args_parser *const restrict _root,
	// _t_args_parser *const restrict _context,
	_t_args_param **opt_param,
	_t_args_option **opt,
	int *const restrict context_type,
	bool *const restrict opt_disabled,
	char *current
)
{
	int	result = error_none;

	/* Defensive recovery */
	if (unlikely(!opt_param || !*opt_param))
	{
		*context_type = args_context_parser;
		*opt = NULL;
		*opt_param = NULL;
		return (result);
	}

	/* "--" always terminates option parsing */
	if (!strcmp(current, "--"))
	{
		*opt_disabled = true;
		*context_type = args_context_parser;
		*opt = NULL;
		*opt_param = NULL;
		return (result);
	}

	/* Stop condition for nargs option parameter:
	 * if this token looks like an option and options are enabled,
	 * it does NOT belong to this option.
	 */
	if (
		!(*opt_disabled) &&
		((*opt_param)->args_type & param_args_type_nargs) &&
		_is_opt(current)
	)
	{
		/* give token back to main loop */
		_root_push_back(_root);

		*context_type = args_context_parser;
		*opt = NULL;
		*opt_param = NULL;
		return (result);
	}

	/* Consume token as option argument */
	result = _add_param(*opt_param, current);
	if (unlikely(result))
	{
		_args_config_set_errnum(result);
		return (result);
	}

	/* Advance param cursor only if this param is not nargs */
	if (!((*opt_param)->args_type & param_args_type_nargs))
		*opt_param = (*opt_param)->next;

	/* Finished all params for this option */
	if (!*opt_param)
	{
		*context_type = args_context_parser;
		*opt = NULL;
	}

	return (result);
}


int	_parse_args(
	_t_args_parser *const restrict _root,
	_t_args_parser *restrict _context
)
{
	int				context_type = args_context_parser;
	bool			opt_disabled = false;
	char			*current = NULL;
	_t_args_parser	*sub = NULL;
	_t_args_param	*pos_cursor = _context->params;	// Cursor for positional params in the current parser context
	_t_args_option	*opt = NULL;					// Current option (when parsing its arguments)
	_t_args_param	*opt_param = NULL;
	int				result = error_none;

	while (_root->index < _root->argc && !result)
	{
		current = _root_pop_next(_root);
		if (!current)
			break;

		/* ---------- Global end-of-options handling ---------- */
		if (context_type == args_context_parser && !strcmp(current, "--"))
		{
			opt_disabled = true;
			continue;
		}

		/* ---------- Parser context ---------- */
		if (context_type == args_context_parser)
			result = _parse_context_parser(&_context, &pos_cursor, &opt_param, &context_type, opt_disabled, current);
		/* ---------- Option-arguments context ---------- */
		else /* context_type == args_context_opt */
			result = _parse_context_opt(_root, &opt_param, &opt, &context_type, &opt_disabled, current);
	}

	return (result);
}

/* ----| Public     |----- */

int	_args_parse(
	ARGSP *const restrict _main,
	const int _argc,
	const char *_argv[]
)
{
	char			*_current = NULL;
	_t_args_parser	*_context = (_t_args_parser *)_main->data.parser;
	_t_args_parser	*_root = _main->data.parser;
	int				result;

	result = _args_check_parser(_root);
	if (unlikely(result))
		goto error;

	_root->index = 1;
	_root->argc = _argc;
	_root->argv = _argv;

	result = _parse_args(_root, _root);

error:
	return (result);
}
