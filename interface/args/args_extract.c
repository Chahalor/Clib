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

char	args_has_param(
	t_args_output *const _parser,
	const char *const _name
)
{
	if (unlikely(!_parser || !_name))
		return (error_invalid_arg);
	else
		return (_args_parser_has_param(_parser, _name));
}

char	args_has_option(
	t_args_output *const _parser,
	const char *const _name
)
{
	if (unlikely(!_parser || !_name))
		return (error_invalid_arg);
	else
		return (_args_parser_has_option(_parser, _name));
}

char	args_has_sub(
	t_args_output *const _parser,
	const char *const _name
)
{
	if (unlikely(!_parser || !_name))
		return (error_invalid_arg);
	else
		return (_args_parser_has_sub(_parser, _name));
}

char	args_option_has_param(
	t_args_output_option *const _option,
	const char *const _name
)
{
	if (unlikely(!_option || !_name))
		return (error_invalid_arg);
	else
		return (_args_option_has_param(_option, _name));
}

char	*args_get_param(
	t_args_output *const _output,
	const char *const _name,
	char **const *const _values,
	unsigned int *const _count
)
{
	if (unlikely(!_output || !_name || !_values || !_count))
		return (error_invalid_arg);
	else
		return (_args_get_param(_output, _name, _values, _count));
}

char	args_get_option(
	t_args_output *const _output,
	const char *const _name,
	char **const *const _values,
	unsigned int *const _count
)
{
	if (unlikely(!_output || !_name || !_values || !_count))
		return (error_invalid_arg);
	else
		return (_args_get_option(_output, _name, _values, _count));
}

const char	*args_active_subcommand(
	const t_args_output *_out
)
{
	if (unlikely(!_out))
		return (NULL);
	else
		return (_args_active_subcommand(_out));
}

t_args_output_parser	*args_get_sub_output(
	const t_args_output *_out
)
{
	if (unlikely(!_out))
		return (NULL);
	else
		return (_args_get_sub_output(_out));
}
