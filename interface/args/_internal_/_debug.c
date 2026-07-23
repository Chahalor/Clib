/**
 * @file _debug.c
 * @brief Human-readable display of an args parser definition.
*/
/* ----| Headers    |----- */
	/* Standard */
#include <stdio.h>

	/* Internal */
#include "_args.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static const char	*_type_name(
	const t_param_type	type
)
{
	if (type == param_type_file)
		return ("file");
	if (type == param_type_int)
		return ("int");
	if (type == param_type_uint)
		return ("uint");
	return ("string");
}

static void	_dump_params(
	const t_args_param	*params,
	const int			pad
)
{
	for (const t_args_param *param = params; param; param = param->next)
	{
		printf("%*sparam <%s> type=%s", pad, "", param->name ? param->name : "(unnamed)", _type_name(param->type));
		if (param->specs & args_param_specs_require)
			printf(" required");

		if (param->specs & args_param_specs_nargs)
			printf(" nargs");

		if (param->desc)
			printf(" — %s", param->desc);

		putchar('\n');
	}
}

static void	_dump_options(
	const t_args_option	*options,
	const int			pad
)
{
	for (const t_args_option *option = options;
		option;
		option = option->next
	)
	{
		printf("%*soption", pad, "");
		if (option->long_name)
			printf(" --%s", option->long_name);

		if (option->short_name)
			printf("%s-%c", option->long_name ? " / " : " ", option->short_name);

		if (option->desc)
			printf(" — %s", option->desc);

		putchar('\n');
		_dump_params(option->params, pad + 2);
	}
}

static void	_dump_parser_tree(
	const t_args_parser	*parser,
	const int			pad
)
{
	for (const t_args_parser *current = parser;
		current;
		current = current->next
	)
	{
		printf("%*sparser %s", pad, "", current->name ? current->name : "(root)");
		if (current->desc)
			printf(" — %s", current->desc);

		putchar('\n');
		_dump_params(current->params, pad + 2);
		_dump_options(current->options, pad + 2);
		if (current->sub_parsers)
		{
			printf("%*ssubcommands:\n", pad + 2, "");
			_dump_parser_tree(current->sub_parsers, pad + 4);
		}
	}
}

/* ----| Public     |----- */

void	_dump_parser(
	t_args_parser	*const	parser
)
{
	if (!parser)
	{
		puts("parser (null)");
		return ;
	}

	_dump_parser_tree(parser, 0);
}
