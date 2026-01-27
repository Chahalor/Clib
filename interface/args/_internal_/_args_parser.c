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

	for (_t_args_parser	*_this = _context->sub_parsers;
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
	const bool		_is_long = _s && _args_is_long_opt(_s);
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

static inline const char	*_root_get_next(
	_t_args_root *const restrict _root
)
{
	return (_root->index < _root->argc ?
				_root->argv[_root->index] :
				NULL
		);
}

static inline const char	*_root_pop_next(
	_t_args_root *const restrict _root
)
{
	return (_root->index < _root->argc ?
				_root->argv[_root->index++] :
				NULL
		);
}

static inline void	_root_push_back(
	_t_args_root *const restrict _root
)
{
	_root->index -= _root->index > 0 ?
						1 :
						0;
}

static _t_args_output_param	*_out_get_param(
	_t_args_output_param *const restrict _list,
	const char *const restrict _name
)
{
	for (_t_args_output_param *_this = _list;
		_this;
		_this = _this->next
	)
	{
		if (_this->name && _name && !strcmp(_this->name, _name))
			return (_this);
	}

	return (NULL);
}

static _t_args_output_param	*_out_add_param(
	_t_args_output_param **const restrict _list,
	const char *const restrict _name
)
{
	_t_args_output_param	*_param = NULL;
	_t_args_output_param	*_last = NULL;

	_param = _out_get_param(*_list, _name);
	if (_param)
		return (_param);

	_param = _args_mem_new_out_param(_name);
	if (unlikely(!_param))
	{
		_args_config_set_errnum(error_alloc_fail);
		return (NULL);
	}

	for (_t_args_output_param *_this = *_list;
		_this;
		_this = _this->next
	)
	{
		_last = _this;
	}

	if (_last)
		_last->next = _param;
	else
		*_list = _param;

	return (_param);
}

static int	_out_add_value(
	_t_args_output_param *const restrict _param,
	const char *const restrict _value
)
{
	_t_args_output_value	*_val = NULL;
	_t_args_output_value	*_last = NULL;

	if (unlikely(!_param || !_value))
		return (args_error_missing_value);

	_val = _args_mem_new_out_val(_value);
	if (unlikely(!_val))
	{
		_args_config_set_errnum(error_alloc_fail);
		return (error_alloc_fail);
	}

	for (_t_args_output_value	*_this = _param->values;
		_this;
		_this = _this->next
	)
	{
		_last = _this;
	}

	if (_last)
		_last->next = _val;
	else
		_param->values = _val;

	return (error_none);
}

static _t_args_output_option	*_out_get_option(
	_t_args_output_option *const restrict _list,
	const _t_args_option *const restrict _opt
)
{
	for (_t_args_output_option *_this = _list;
		_this;
		_this = _this->next
	)
	{
		if (_opt->long_name && _this->long_name &&
			!strcmp(_this->long_name, _opt->long_name))
			return (_this);
		if (!_opt->long_name && _opt->short_name &&
			_this->short_name == _opt->short_name)
			return (_this);
		if (_opt->short_name && !_this->long_name &&
			_this->short_name == _opt->short_name)
			return (_this);
	}

	return (NULL);
}

static _t_args_output_option	*_out_add_option(
	_t_args_output_option **const restrict _list,
	const _t_args_option *const restrict _opt
)
{
	_t_args_output_option	*_out = NULL;
	_t_args_output_option	*_last = NULL;

	_out = _out_get_option(*_list, _opt);
	if (_out)
		return (_out);

	_out = _args_mem_new_out_opt(_opt);
	if (unlikely(!_out))
	{
		_args_config_set_errnum(error_alloc_fail);
		return (NULL);
	}

	for (_t_args_output_option *_this = *_list;
		_this;
		_this = _this->next
	)
	{
		_last = _this;
	}

	if (_last)
		_last->next = _out;
	else
		*_list = _out;

	return (_out);
}

static _t_args_output_parser	*_out_add_sub_parser(
	_t_args_output_parser *const restrict _parent,
	const _t_args_parser *const restrict _sub
)
{
	_t_args_output_parser	*_out = NULL;
	_t_args_output_parser	*_last = NULL;

	_out = _args_mem_new_out_parser(_sub->name);
	if (unlikely(!_out))
	{
		_args_config_set_errnum(error_alloc_fail);
		return (NULL);
	}

	for (_t_args_output_parser *_this = _parent->sub;
		_this;
		_this = _this->next
	)
	{
		_last = _this;
	}

	if (_last)
		_last->next = _out;
	else
		_parent->sub = _out;

	return (_out);
}

static inline int	_args_parse_loop_parser(
	const _t_args_parser **context,
	const char *const current,
	_t_args_param **pos_cursor,
	_t_args_output_parser **out_context,
	const bool opt_disabled,
	_t_args_option **opt,
	_t_args_output_option **out_opt,
	_t_args_param **opt_param,
	enum e_args_context_type *context_type
)
{
	_t_args_parser			*sub = NULL;
	_t_args_output_param	*out_param = NULL;
	int						result = error_none;

	sub = _get_sub_parser_of(*context, current);
	if (sub)
	{
		*context = sub;
		*pos_cursor = (*context)->params;
		*out_context = _out_add_sub_parser(*out_context, sub);
		if (unlikely(!*out_context))
		{
			result = error_alloc_fail;
			goto error;
		}
		goto error;
	}

	if (!opt_disabled && _args_is_opt(current))
	{
		*opt = _get_opt_of(*context, current);
		if (unlikely(!*opt))
		{
			result = args_error_unknown_option;
			_args_config_set_errnum(result);
			goto error;
		}

		*out_opt = _out_add_option(&(*out_context)->options, *opt);
		if (unlikely(!*out_opt))
		{
			result = error_alloc_fail;
			goto error;
		}
		(*out_opt)->nb_call += 1;

		*opt_param = (*opt)->params;
		if (*opt_param)
			*context_type = args_context_opt;
		goto error;
	}

	if (unlikely(!*pos_cursor))
	{
		result = args_error_extra_param;
		_args_config_set_errnum(result);
		goto error;
	}

	out_param = _out_add_param(&(*out_context)->params, (*pos_cursor)->name);
	if (unlikely(!out_param))
	{
		result = error_alloc_fail;
		goto error;
	}

	result = _out_add_value(out_param, current);
	if (unlikely(result))
		goto error;

	if (!((*pos_cursor)->specs & args_param_specs_nargs))
		*pos_cursor = (*pos_cursor)->next;

error:
	return (result);
}

static inline int	_args_parse_loop_option(
	_t_args_root *const _root,
	const char *const current,
	bool *const opt_disabled,
	_t_args_option **opt,
	_t_args_output_option **out_opt,
	_t_args_param **opt_param,
	enum e_args_context_type *context_type
)
{
	_t_args_output_param	*out_param = NULL;
	int						result = error_none;

	if (unlikely(!opt_param || !out_opt || !*opt_param || !*out_opt))
	{
		*context_type = args_context_parser;
		*opt = NULL;
		*out_opt = NULL;
		*opt_param = NULL;
		goto error;
	}

	if (!strcmp(current, "--"))
	{
		*opt_disabled = true;
		*context_type = args_context_parser;
		*opt = NULL;
		*out_opt = NULL;
		*opt_param = NULL;
		goto error;
	}

	if (
		!*opt_disabled &&
		((*opt_param)->specs & args_param_specs_nargs) &&
		_args_is_opt(current)
	)
	{
		_root_push_back(_root);
		*context_type = args_context_parser;
		*opt = NULL;
		*out_opt = NULL;
		*opt_param = NULL;
		goto error;
	}

	out_param = _out_add_param(&(*out_opt)->params, (*opt_param)->name);
	if (unlikely(!out_param))
	{
		result = error_alloc_fail;
		goto error;
	}

	result = _out_add_value(out_param, current);
	if (unlikely(result))
		goto error;

	if (!((*opt_param)->specs & args_param_specs_nargs))
		*opt_param = (*opt_param)->next;

	if (!*opt_param)
	{
		*context_type = args_context_parser;
		*opt = NULL;
		*out_opt = NULL;
	}

error:
	return (result);
}

int	_args_parse_loop(
	_t_args_root *const _root,
	_t_args_output *const _out
)
{
	enum e_args_context_type	context_type = args_context_parser;
	bool						opt_disabled = false;
	const char					*current = NULL;
	const _t_args_parser		*context = _root->parser;
	_t_args_output_parser		*out_context = _out->root;
	_t_args_param				*pos_cursor = context ? context->params : NULL;
	_t_args_option				*opt = NULL;
	_t_args_output_option		*out_opt = NULL;
	_t_args_param				*opt_param = NULL;
	int							result = error_none;

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
		{
			result = _args_parse_loop_parser(
				&context,
				current,
				&pos_cursor,
				&out_context,
				opt_disabled,
				&opt,
				&out_opt,
				&opt_param,
				&context_type
			);
		}
		/* ---------- Option-arguments context ---------- */
		else /* context_type == args_context_opt */
		{
			result = _args_parse_loop_option(
				_root,
				current,
				&opt_disabled,
				&opt,
				&out_opt,
				&opt_param,
				&context_type
			);
		}
	}

	_out->error = result;
	return (result);
}

/* ----| Public     |----- */

_t_args_output	*_args_parse(
	const t_args_parser *const _parser,
	const int _argc,
	const char *const *const _argv
)
{
	_t_args_root	_root = {
		.parser = _parser,
		.argc = _argc,
		.argv = _argv,
		.index = 1
	};
	int				_errnum;
	_t_args_output	*result;

	result = _args_mem_new_output();
	if (unlikely(!result))
		return (NULL);

	result->root = _args_mem_new_out_parser(_parser ? _parser->name : NULL);
	if (unlikely(!result->root))
	{
		mem_free(result);
		return (NULL);
	}

	_errnum = _args_parse_loop(&_root, result);

	if (likely(!_errnum))
		_errnum = _args_check_output(&_root, result);

	if (_errnum)
		_args_config_set_errnum(_errnum);

	result->error = _errnum;

error:
	return (result);
}
