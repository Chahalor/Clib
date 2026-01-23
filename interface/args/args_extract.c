// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "_internal_/_args.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

int	args_has(
	const ARGSP *const restrict parser,
	const char *const restrict field
)
{
	int	result;

	if (unlikely(!parser || !field))
		result = error_invalid_arg;
	else if (parser->type == _e_args_data_type_parser || parser->type == _e_args_data_type_root)
		result = _args_check_exist_parser(parser->data.parser, field);
	else
		result = error_invalid_arg;

	return (result);
}

int	args_has_opt(
	const ARGSP *const restrict parser,
	const char *const restrict field
)
{
	int	result;

	if (unlikely(!parser || !field))
		result = error_invalid_arg;
	else if (parser->type == _e_args_data_type_parser || parser->type == _e_args_data_type_root)
		result = _args_check_opt_exist_parser(parser->data.parser, field);
	else
		result = error_invalid_arg;

	return (result);
}

int				args_has_param(
	const ARGSP *const restrict parser,
	const char *const restrict field
)
{
	int	result;

	if (unlikely(!parser || !field))
		result = error_invalid_arg;
	else if (parser->type == _e_args_data_type_parser || parser->type == _e_args_data_type_root)
		result = _args_check_param_exist_parser(parser->data.parser, field);
	else
		result = error_invalid_arg;

	return (result);
}

int	args_get(
	const ARGSP *const restrict parser,
	const char *const restrict field,
	void *const restrict dest
)
{
	int	result;

	if (unlikely(!parser || !field || !dest))
		result = error_invalid_arg;
	else if (parser->type == _e_args_data_type_root || parser->type == _e_args_data_type_parser)
	{
		result = _args_get_opt(parser->data.parser->options, field, dest);
		if (field[0] == '-')
			goto exit;
		result = !result ?
					result :
					_args_get_sub(parser->data.parser->sub_parsers, field, dest);
		result = !result ?
			result :
			_args_get_param(parser->data.parser->params, field, dest);
	}
	else if (parser->type == _e_args_data_type_opt)
		result = _args_get_opt(parser->data.option, field, dest);
	else if (parser->type == _e_args_data_type_param)
		result = _args_get_param(parser->data.param, field, dest);
	else
		result = error_invalid_arg;

exit:
	return (result);
}

int	args_get_opt(
	const ARGSP *const restrict parser,
	const char *const restrict field,
	char *const restrict dest
)
{
	int	result;

	if (unlikely(!parser || !field || !dest))
		result = error_invalid_arg;
	else
		result = _args_get_opt(parser->data.parser->options, field, dest);

	return (result);
}

int	args_get_param(
	const ARGSP *const restrict parser,
	const char *const restrict field,
	char *const restrict dest
)
{
	int	result;

	if (unlikely(!parser || !field || !dest))
		result = error_invalid_arg;
	else
		result = _args_get_param(parser->data.parser->params, field, dest);

	return (result);
}

int	args_get_sub_parser(
	const ARGSP *const restrict parser,
	const char *const restrict field,
	ARGS_SUB_PARSER *const restrict dest
)
{
	int	result;

	if (unlikely(!parser || !field || !dest))
		result = error_invalid_arg;
	else
		result = _args_get_sub(parser->data.parser->sub_parsers, field, dest);

	return (result);
}
