// Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdio.h>
#include <stdlib.h>

	/* Internal */
#include "_args.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static void	_print_params(
	const _t_args_param *p
)
{
	while (p)
	{
		printf("  %s", p->name);
		if (p->specs & args_param_specs_nargs)
			printf("...");
		if (p->specs & args_param_specs_require)
			printf(" (required)");
		if (p->desc)
			printf("\n      %s", p->desc);
		printf("\n");
		p = p->next;
	}
}

static void	_print_options(
	const _t_args_option *o
)
{
	while (o)
	{
		printf("  ");
		if (o->short_name)
			printf("-%c", o->short_name);
		if (o->long_name)
		{
			if (o->short_name)
				printf(", ");
			printf("--%s", o->long_name);
		}
		printf("\n");
		if (o->desc)
			printf("      %s\n", o->desc);

		if (o->params)
		{
			printf("      params:\n");
			_print_params(o->params);
		}
		o = o->next;
	}
}

static void	_print_subparsers(
	const _t_args_parser *const p
)
{
	for (const _t_args_parser	*sub = p->sub_parsers;
		sub;
		sub = sub->next
	)
	{
		printf("  %s\n", sub->name);
		if (sub->desc)
			printf("      %s\n", sub->desc);
	}
}

/* ----| Public     |----- */

int	_args_builtin_help(
	const _t_args_parser *const _parser,
	const _t_args_config *const _config
)
{
	if (!_parser /* || !_config */)
		return (args_error_internal);

	// if (!_config->auto_help)
	// 	return (args_error_none);

	/* ---------- Header ---------- */
	printf("Usage:\n");
	printf("  %s", _parser->name ? _parser->name : "program");

	if (_parser->options)
		printf(" [options]");
	if (_parser->params)
		printf(" [params]");
	if (_parser->sub_parsers)
		printf(" <command>");

	printf("\n\n");

	/* ---------- Description ---------- */
	if (_parser->desc)
	{
		printf("Description:\n");
		printf("  %s\n\n", _parser->desc);
	}

	/* ---------- Options ---------- */
	if (_parser->options)
	{
		printf("Options:\n");
		_print_options(_parser->options);
		printf("\n");
	}

	/* ---------- Params ---------- */
	if (_parser->params)
	{
		printf("Parameters:\n");
		_print_params(_parser->params);
		printf("\n");
	}

	/* ---------- Sub-commands ---------- */
	if (_parser->sub_parsers)
	{
		printf("Commands:\n");
		_print_subparsers(_parser);
		printf("\n");
	}

	// /* ---------- Exit policy ---------- */
	// if (_config->exit_on_help)
		exit(0);

	return (args_error_none);
}
