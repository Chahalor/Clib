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
	t_args_parser	*result = NULL;

	result = _args_parser_add_sub(_parent, _name, _desc);

	return (result);
}

#pragma region Options

t_args_option	*args_parser_add_option(
	t_args_parser *const _parser,
	const char *const _long_name,
	const char _short_name,
	const char *const _desc
)
{
	t_args_option	*result = NULL;

	if (unlikely(!_long_name && !_short_name))
		result = NULL;
	else
		result = _args_parser_add_option(_parser, _long_name, _short_name, _desc);

	return (result);
}


#pragma region Parameters

t_args_param	*args_parser_add_param(
	t_args_parser *const _parser,
	const char *const _name,
	const char *const _desc,
	const int _spec,
	const int _type
)
{
	t_args_param	*result = NULL;

	result = _args_parser_add_param(_parser, _name, _desc, _spec, _type);

	return (result);
}

t_args_param	*args_option_add_param(
	t_args_option *const _parent,
	const char *const _name,
	const char *const _desc,
	const int _spec,
	const int _type
)
{
	t_args_param	*result = NULL;

	result = _args_option_add_param(_parent, _name, _desc, _spec, _type);

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
