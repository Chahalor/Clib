// Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdarg.h>

	/* Internal */
#include "../args.h"
#include "_internal_/_args.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */


ARGS_SUB_PARSER	*args_add_subparser(
	ARGSP *const parent,
	const char *const name
)
{
	int				_error;
	ARGS_SUB_PARSER	*result = NULL;

	if (unlikely(!parent || !name ||
				(parent->type != _e_args_data_type_parser && parent->type != _e_args_data_type_root)))
		result = NULL;
	else
		_error = _args_add_sub_parser(parent->data.parser, name, &result);

	return (result);
}

ARGS_SUB_PARSER	*args_parser_add_subparser(
	ARGSP *const parent,
	const char *const name
)
{
	return (args_add_subparser(parent, name));
}

ARGS_PARAM		*args_parser_add_param(
	ARGSP *const parser,
	const char *const name,
	const t_param_type type,
	const t_param_args_type spec
)
{
	int			_error;
	ARGS_PARAM	*result = NULL;

	if (unlikely(!parser || !name || (parser->type == _e_args_data_type_param)))
	{
		result = NULL;
		goto error;
	}

	switch (parser->type)
	{
	case (_e_args_data_type_opt):
		_error = _args_add_param_to_option(parser->data.option, name, &result);
		if (unlikely(_error))
			goto error;
		break;

	case (_e_args_data_type_parser):
	case (_e_args_data_type_root):
		_error = _args_add_param_to_parser(parser->data.parser, name, &result);
		if (unlikely(_error))
			goto error;
		break;

	default:
		result = NULL;
		break;
	}

	if (result)
	{
		result->data.param->type = type;
		result->data.param->args_type = spec;
	}

error:
	return (result);
}

ARGS_PARAM		*args_option_add_param(
	ARGS_OPT *const option,
	const char *const name,
	const t_param_type type,
	const t_param_args_type spec
)
{
	int			_error;
	ARGS_PARAM	*result = NULL;

	if (unlikely(!option || !name || option->type != _e_args_data_type_opt))
	{
		result = NULL;
		goto error;
	}

	_error = _args_add_param_to_option(option->data.option, name, &result);
	if (unlikely(_error))
		goto error;
	if (result)
	{
		result->data.param->type = type;
		result->data.param->args_type = spec;
	}

error:
	return (result);
}

ARGS_OPT		*args_add_option(
	ARGSP *const parser,
	const char *const long_name,
	const char short_name
)
{
	int			_error;
	ARGS_OPT	*result;

	if (unlikely(!parser || (!long_name && !short_name) ||
				(parser->type != _e_args_data_type_parser && parser->type != _e_args_data_type_root)))
	{
		result = NULL;
		goto error;
	}
	else
		_error = _args_add_option(parser->data.parser, long_name, short_name, &result);

error:
	return (result);
}

ARGS_OPT		*args_parser_add_option(
	ARGSP *const parser,
	const char *const long_name,
	const char short_name
)
{
	return (args_add_option(parser, long_name, short_name));
}

int	args_set_desc(
	ARGSP *const target,
	const char *const fmt,
	...
)
{
	va_list	_list;
	int		result;

	if (unlikely(!target || !fmt))
	{
		result = error_invalid_arg;
		goto error;
	}

	va_start(_list, fmt);
	switch (target->type)
	{
	case (_e_args_data_type_root):
	case (_e_args_data_type_parser):
		result = _args_set_desc_parser(target->data.parser, _args_set_desc_add, fmt, &_list);
		break ;
	case (_e_args_data_type_param):
		result = _args_set_desc_param(target->data.param, _args_set_desc_add, fmt, &_list);
		break ;
	case (_e_args_data_type_opt):
		result = _args_set_desc_option(target->data.option, _args_set_desc_add, fmt, &_list);
		break ;
	
	default:
		result = error_invalid_arg;
		break;
	}

error:
	return (result);
}
