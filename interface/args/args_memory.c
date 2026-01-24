// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

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

/** */
t_args_parser	*args_parser_new(void)
{
	return (_args_mem_new_parser(NULL, NULL));
}

#pragma region Sub-parsers

/** */
t_args_parser	*args_parser_add_sub(
	t_args_parser *const _parent,
	const char *const _name,
	const char *const _desc
)
{
	int	result = error_none;

	if (unlikely(!_parent || !_name || !_desc))
		result = error_invalid_arg;
	else
		result = _args_parser_add_sub(_parent, _name, _desc);

	return (result);
}

#pragma region Options

t_args_option	*args_parser_add_option(
	t_args_parser *const _parser,
	const char *const _name,
	const char *const _desc
)
{
	int	result = error_none;

	if (unlikely(!_parser || !_name || !_desc))
		result = error_invalid_arg;
	else
		result = _args_parser_add_sub(_parser, _name, _desc);

	return (result);
}


#pragma region Parameters

t_args_param	*args_parser_add_param(
	t_args_parser *const _parser,
	const char *const _name,
	const char *const _desc
)
{
	int	result = error_none;

	if (unlikely(!_parser || !_name || !_desc))
		result = error_invalid_arg;
	else
		result = _args_parser_add_sub(_parser, _name, _desc);

	return (result);
}


/** */
void			args_parser_free(
	t_args_parser *const _parser
)
{
	if (likely(_parser))
		_args_mem_free_parser(_parser, true);
}

void			args_output_free(
	t_args_output *const _out
)
{
	if (likely(_out))
		_args_mem_free_output(_out, true);
}
