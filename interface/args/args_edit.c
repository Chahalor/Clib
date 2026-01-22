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
	ARGS_PARSER *const parent,
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

ARGS_PARAM		*args_parser_add_param(
	ARGS_PARSER *const parser,
	const char *const name
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
		break;

	case (_e_args_data_type_parser):
	case (_e_args_data_type_root):
		_error = _args_add_param_to_parser(parser->data.parser, name, &result);

	default:
		result = NULL;
		break;
	}

error:
	return (result);
}

ARGS_OPT		*args_add_option(
	ARGS_PARSER *const parser,
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

int	args_set_desc(
	ARGS_PARSER *const target,
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
		result = _args_add_desc_parser(target->data.parser, fmt, &_list);
		break ;
	case (_e_args_data_type_param):
		result = _args_add_desc_param(target->data.param, fmt, &_list);
		break ;
	case (_e_args_data_type_opt):
		result = _args_add_desc_option(target->data.option, fmt, &_list);
		break ;
	
	default:
		result = error_invalid_arg;
		break;
	}

error:
	return (result);
}

