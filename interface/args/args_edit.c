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


ARGS_SUB_PARSER	*args_parser_add_subparser(
	ARGS_PARSER *const parent,
	const char *const name
)
{
	ARGS_SUB_PARSER	*result = NULL;

	if (unlikely(!parent || !name))
		result = NULL;
	else
		result = _args_add_sub_parser(parent, name);

	return (result);
}

ARGS_PARAM		*args_parser_add_param(
	ARGS_PARSER *const parser,
	const char *const name,
	const t_param_type type,
	const t_param_args_type spec
)
{
	ARGS_PARAM	*result = NULL;

	if (unlikely(!parser || !name))
		result = NULL;
	else
		result = _args_add_param(parser, name, type, spec);

	return (result);
}

ARGS_OPT		*args_parser_add_option(
	ARGS_PARSER *const parser,
	const char *const long_name,
	const char short_name
)
{
	ARGS_OPT	*result;

	if (unlikely(!parser || (!long_name && !short_name)))
		result = NULL;
	else
		result = _args_add_opt(parser, long_name, short_name);

	return (result);
}

ARGS_PARAM		*args_option_add_param(
	ARGS_OPT *const option,
	const char *const name,
	const t_param_type type,
	const t_param_args_type spec
)
{
	ARGS_PARAM	*result;

	if (unlikely(!option || !name))
}

int				args_param_set_desc(
					ARGS_PARAM *const param,
					const char *const fmt,
					...
				);

int				args_option_set_desc(
					ARGS_OPT *const option,
					const char *const fmt,
					...
				);

int				args_parser_set_desc(
					ARGS_PARSER *const parser,
					const char *const fmt,
					...
				);