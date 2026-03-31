/**
 * @file main.c
 * @brief Controls for the public API of interface/args.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../args.h"
#include "../../../memory.h"
#include "../../../standards/formating.h"

#define VALUE_SENTINEL ((char **)0x1)

static unsigned int	g_checks = 0;
static unsigned int	g_failures = 0;

#define CHECK(_cond, _msg)														\
	do																		\
	{																		\
		g_checks++;															\
		if ((_cond))														\
			printf("[" GREEN "PASS" RESET"] %s\n", _msg);									\
		else																\
		{																	\
			g_failures++;													\
			fprintf(stderr, "[" RED "FAIL" RESET "] %s:%d: %s\n", __FILE__, __LINE__, _msg);\
		}																	\
	} while (0)

static void	_free_values(
	char **values
)
{
	if (values && values != VALUE_SENTINEL)
		mem_free(values);
}

static t_args_parser	*_build_full_parser(void)
{
	t_args_parser	*parser;
	t_args_option	*opt;
	t_args_parser	*sub;

	parser = args_parser_new();
	if (!parser)
		return (NULL);

	opt = args_parser_add_option(parser, "config", 'c', "config file");
	if (!opt || !args_option_add_param(opt, "path", "config path",
			args_param_specs_require, param_type_file))
		goto fail;

	if (!args_parser_add_option(parser, "verbose", 'v', "verbose output"))
		goto fail;
	if (!args_parser_add_param(parser, "input", "input file",
			args_param_specs_require, param_type_file))
		goto fail;
	if (!args_parser_add_param(parser, "extras", "extra args",
			args_param_specs_nargs, param_type_str))
		goto fail;

	sub = args_parser_add_sub(parser, "pull", "pull command");
	if (!sub)
		goto fail;
	if (!args_parser_add_param(sub, "repo", "repository",
			args_param_specs_require, param_type_str))
		goto fail;
	opt = args_parser_add_option(sub, "todo", 't', "todo file");
	if (!opt || !args_option_add_param(opt, "file", "todo path",
			args_param_specs_require, param_type_file))
		goto fail;
	if (!args_parser_add_option(sub, "force", 'f', "force action"))
		goto fail;
	return (parser);
fail:
	args_parser_free(parser);
	return (NULL);
}

static t_args_parser	*_build_error_parser(void)
{
	t_args_parser	*parser;
	t_args_option	*opt;

	parser = args_parser_new();
	if (!parser)
		return (NULL);
	opt = args_parser_add_option(parser, "config", 'c', "config file");
	if (!opt || !args_option_add_param(opt, "path", "config path",
			args_param_specs_require, param_type_file))
		goto fail;
	if (!args_parser_add_option(parser, "verbose", 'v', "verbose output"))
		goto fail;
	if (!args_parser_add_param(parser, "input", "input file",
			args_param_specs_require, param_type_file))
		goto fail;
	return (parser);
fail:
	args_parser_free(parser);
	return (NULL);
}

static t_args_parser	*_build_cluster_parser(void)
{
	t_args_parser	*parser;
	t_args_option	*opt;

	parser = args_parser_new();
	if (!parser)
		return (NULL);
	if (!args_parser_add_option(parser, "alpha", 'a', "alpha flag"))
		goto fail;
	if (!args_parser_add_option(parser, "beta", 'b', "beta flag"))
		goto fail;
	opt = args_parser_add_option(parser, "config", 'c', "config file");
	if (!opt || !args_option_add_param(opt, "path", "config path",
			args_param_specs_require, param_type_file))
		goto fail;
	if (!args_parser_add_param(parser, "first", "first positional",
			args_param_specs_require, param_type_str))
		goto fail;
	return (parser);
fail:
	args_parser_free(parser);
	return (NULL);
}

static void	_test_public_api_basic(void)
{
	t_args_parser	*parser;
	t_args_option	*opt;
	t_args_param	*param;
	char			**values;
	unsigned int	count;
	const char		*argv[] = {"args-controls"};

	CHECK(args_parse(NULL, 1, argv)					== NULL,				"args_parse(NULL, ...) must fail");
	CHECK(args_error(NULL)							== args_error_internal,	"args_error(NULL) must be internal");
	CHECK(args_error_str(args_error_unknown_option)	!= NULL,				"args_error_str must return text");
	CHECK(args_parser_set_desc(NULL, "x")			== error_invalid_arg,	"parser set desc null guard");
	CHECK(args_param_set_desc(NULL, "x")			== error_invalid_arg,	"param set desc null guard");
	CHECK(args_option_set_desc(NULL, "x")			== error_invalid_arg,	"option set desc null guard");
	CHECK(args_has_option(NULL, "x")				== error_invalid_arg,	"has_option null guard");
	CHECK(args_has_param(NULL, "x")					== error_invalid_arg,	"has_param null guard");
	CHECK(args_has_sub(NULL, "x")					== error_invalid_arg,	"has_sub null guard");
	CHECK(args_option_has_param(NULL, "x")			== error_invalid_arg,	"option_has_param null guard");

	values = NULL;
	count = 0;
	CHECK(args_get_option(NULL, "x", (void *)&values, &count)				== error_invalid_arg,	"get_option null output guard");
	CHECK(args_get_param(NULL, "x", (void *)&values, &count)				== NULL,				"get_param null output guard");
	CHECK(args_output_parser_get_option(NULL, "x", (void *)&values, &count)	== error_invalid_arg,	"sub get_option null output guard");
	CHECK(args_output_parser_get_param(NULL, "x", (void *)&values, &count)	== NULL,				"sub get_param null output guard");

	parser = args_parser_new();
	CHECK(parser															!= NULL,				"args_parser_new should allocate parser");
	if (!parser)
		return ;

	opt = args_parser_add_option(parser, "sample", 's', "sample option");
	param = args_parser_add_param(parser, "input", "input param", args_param_specs_require, param_type_str);
	CHECK(opt	= NULL,		"add option should succeed");
	CHECK(param	!= NULL,	"add parser param should succeed");

	if (opt)
		CHECK(args_option_add_param(opt, "value", "sample value", args_param_specs_require, param_type_str) != NULL, "add option param should succeed");

	CHECK(args_parser_set_desc(parser, "updated parser")	== error_none, "parser desc should be editable");
	if (param)
		CHECK(args_param_set_desc(param, "updated param")	== error_none, "param desc should be editable");
	if (opt)
		CHECK(args_option_set_desc(opt, "updated option")	== error_none, "option desc should be editable");

	args_parser_free(parser);
}

static void	_test_root_parse_output(void)
{
	t_args_parser	*parser;
	t_args_output	*out;
	char			**values;
	unsigned int	count;
	char			*name;
	const char		*argv[] = {
		"args-controls",
		"--config", "settings.json",
		"-v",
		"input.txt", "extra1", "extra2"
	};
	const int		argc = (int)(sizeof(argv) / sizeof(argv[0]));

	parser = _build_full_parser();
	CHECK(parser != NULL, "root parser setup should succeed");
	if (!parser)
		return ;

	out = args_parse(parser, argc, argv);
	CHECK(out != NULL, "parse should return output");
	if (!out)
	{
		args_parser_free(parser);
		return ;
	}

	CHECK(args_error(out)					== args_error_none,	"valid root parse should have no error");
	CHECK(args_has_option(out, "config")	== 1,				"config option should be present");
	CHECK(args_has_option(out, "-v")		== 1,				"verbose option should be present");
	CHECK(args_has_param(out, "input")		== 1,				"input param should be present");
	CHECK(args_has_param(out, "extras")		== 1,				"extras param should be present");
	CHECK(args_has_sub(out, "pull")			== 0,				"no subcommand should be selected");
	CHECK(args_active_subcommand(out)		== NULL,			"active subcommand should be null");
	CHECK(args_get_sub_output(out)			== NULL,			"sub output should be null");

	values = NULL;
	count = 0;
	CHECK(args_get_option(out, "config", (void *)&values, &count)	== 1,	"config option extraction should succeed");
	CHECK(count														== 1,	"config option should have one value");
	CHECK(values && !strcmp(values[0], "settings.json"),					"config value should match");
	_free_values(values);

	values = NULL;
	count = 0;
	CHECK(args_get_option(out, "verbose", (void *)&values, &count)	== 1,				"verbose option extraction should succeed");
	CHECK(values													== VALUE_SENTINEL,	"flag option should return sentinel pointer");
	CHECK(count														== 0,				"flag option should return zero value count");
	_free_values(values);

	values = NULL;
	count = 0;
	name = args_get_param(out, "input", (void *)&values, &count);
	CHECK(name && !strcmp(name, "input"),				"input param extraction should find input");
	CHECK(count == 1,									"input should contain exactly one value");
	CHECK(values && !strcmp(values[0], "input.txt"),	"input value should match");
	_free_values(values);

	values = NULL;
	count = 0;
	name = args_get_param(out, "extras", (void *)&values, &count);
	CHECK(name && !strcmp(name, "extras"),			"extras param extraction should find extras");
	CHECK(count == 2,								"extras should contain two values");
	CHECK(values && !strcmp(values[0], "extra1"),	"extras first value should match");
	CHECK(values && !strcmp(values[1], "extra2"),	"extras second value should match");
	_free_values(values);

	values = VALUE_SENTINEL;
	count = 42;
	CHECK(args_get_option(out, "missing", (void *)&values, &count)	== 0,					"missing option should not be found");
	CHECK(values													== NULL && count == 0,	"missing option should reset output pointers");

	args_output_free(out);
	args_parser_free(parser);
}

static void	_test_subcommand_output(void)
{
	t_args_parser			*parser;
	t_args_output			*out;
	t_args_output_parser	*sub;
	char					**values;
	unsigned int			count;
	char					*name;
	const char				*argv[] = {
		"args-controls",
		"--config", "settings.json",
		"pull", "origin/main",
		"--todo", "todo.md",
		"-f"
	};
	const int				argc = (int)(sizeof(argv) / sizeof(argv[0]));

	parser = _build_full_parser();
	CHECK(parser != NULL, "sub parser setup should succeed");
	if (!parser)
		return ;

	out = args_parse(parser, argc, argv);
	CHECK(out != NULL, "sub parse should return output");
	if (!out)
	{
		args_parser_free(parser);
		return ;
	}

	CHECK(args_error(out)					== args_error_none,							"valid sub parse should have no error");
	CHECK(args_has_option(out, "config")	== 1,										"root config should still be present");
	CHECK(args_has_sub(out, "pull")			== 1,										"pull subcommand should be present");
	CHECK(args_active_subcommand(out) && !strcmp(args_active_subcommand(out), "pull"),	"active subcommand should be pull");

	sub = args_get_sub_output(out);
	CHECK(sub != NULL, "sub output should be returned");
	if (!sub)
	{
		args_output_free(out);
		args_parser_free(parser);
		return ;
	}

	values = NULL;
	count = 0;
	name = args_output_parser_get_param(sub, "repo", (void *)&values, &count);
	CHECK(name && !strcmp(name, "repo"),				"repo sub param should be present");
	CHECK(count == 1,									"repo should contain one value");
	CHECK(values && !strcmp(values[0], "origin/main"),	"repo value should match");
	_free_values(values);

	values = NULL;
	count = 0;
	CHECK(args_output_parser_get_option(sub, "todo", (void *)&values, &count) == 1,	"sub todo option should be present");
	CHECK(count == 1,																"sub todo option should contain one value");
	CHECK(values && !strcmp(values[0], "todo.md"),									"sub todo value should match");
	_free_values(values);

	values = NULL;
	count = 0;
	CHECK(args_output_parser_get_option(sub, "force", (void *)&values, &count) == 1,	"sub force option should be present");
	CHECK(values	== VALUE_SENTINEL,													"sub flag option should return sentinel pointer");
	CHECK(count		== 0,																"sub flag option should return zero value count");
	_free_values(values);

	values = VALUE_SENTINEL;
	count = 123;
	CHECK(args_output_parser_get_option(sub, "config", (void *)&values, &count) == 0,	"sub should not expose root config option");
	CHECK(values == NULL && count == 0,													"missing sub option should reset output pointers");

	args_output_free(out);
	args_parser_free(parser);
}

static void	_test_error_paths(void)
{
	t_args_parser	*parser;
	t_args_output	*out;
	const char		*argv_unknown[] = {"args-controls", "--ghost"};
	const char		*argv_missing_opt_value[] = {"args-controls", "--config"};
	const char		*argv_missing_param[] = {"args-controls", "-v"};
	const char		*argv_extra_param[] = {"args-controls", "-v", "one", "two"};

	parser = _build_error_parser();
	CHECK(parser != NULL, "error parser setup should succeed");
	if (!parser)
		return ;

	out = args_parse(parser, (int)(sizeof(argv_unknown) / sizeof(argv_unknown[0])), argv_unknown);
	CHECK(out != NULL, "unknown option parse should return output");
	if (out)
	{
		CHECK(args_error(out) == args_error_unknown_option, "unknown option should set args_error_unknown_option");
		args_output_free(out);
	}

	out = args_parse(parser,
			(int)(sizeof(argv_missing_opt_value) / sizeof(argv_missing_opt_value[0])),
			argv_missing_opt_value);
	CHECK(out != NULL, "missing option value parse should return output");
	if (out)
	{
		CHECK(args_error(out) == args_error_missing_param,"missing option value should set args_error_missing_param");
		args_output_free(out);
	}

	out = args_parse(parser,
			(int)(sizeof(argv_missing_param) / sizeof(argv_missing_param[0])),
			argv_missing_param);
	CHECK(out != NULL, "missing required param parse should return output");
	if (out)
	{
		CHECK(args_error(out) == args_error_missing_param,
			"missing required param should set args_error_missing_param");
		args_output_free(out);
	}

	out = args_parse(parser,
			(int)(sizeof(argv_extra_param) / sizeof(argv_extra_param[0])),
			argv_extra_param);
	CHECK(out != NULL, "extra param parse should return output");
	if (out)
	{
		CHECK(args_error(out) == args_error_extra_param,
			"extra param should set args_error_extra_param");
		args_output_free(out);
	}

	args_parser_free(parser);
}

static void	_test_basic_parsing_controls(void)
{
	t_args_parser	*parser;
	t_args_output	*out;
	char			**values;
	unsigned int	count;
	char			*name;
	const char		*argv[] = {
		"args-controls",
		"-ab",
		"--config", "conf.yml",
		"--",
		"-c"
	};
	const int		argc = (int)(sizeof(argv) / sizeof(argv[0]));

	parser = _build_cluster_parser();
	CHECK(parser != NULL, "cluster parser setup should succeed");
	if (!parser)
		return ;

	out = args_parse(parser, argc, argv);
	CHECK(out != NULL, "cluster parse should return output");
	if (!out)
	{
		args_parser_free(parser);
		return ;
	}

	CHECK(args_error(out) == args_error_none, "cluster/delimiter parse should succeed");
	CHECK(args_has_option(out, "alpha") == 1, "alpha option from -ab should be present");
	CHECK(args_has_option(out, "beta") == 1, "beta option from -ab should be present");
	CHECK(args_has_option(out, "config") == 1, "config option should be present");

	values = NULL;
	count = 0;
	CHECK(args_get_option(out, "config", (void *)&values, &count) == 1,
		"config option extraction should succeed");
	CHECK(count == 1, "config option should keep exactly one value");
	CHECK(values && !strcmp(values[0], "conf.yml"), "config value should match");
	_free_values(values);

	values = NULL;
	count = 0;
	name = args_get_param(out, "first", (void *)&values, &count);
	CHECK(name && !strcmp(name, "first"), "first positional param should be present");
	CHECK(count == 1, "first positional should contain one value");
	CHECK(values && !strcmp(values[0], "-c"),
		"delimiter should keep '-c' as a positional value");
	_free_values(values);

	args_output_free(out);
	args_parser_free(parser);
}

int	main(void)
{
	_test_public_api_basic();
	_test_root_parse_output();
	_test_subcommand_output();
	_test_error_paths();
	_test_basic_parsing_controls();

	if (g_failures)
	{
		fprintf(stderr, "\n[" RED "FAIL" RESET"] %u/%u controls failed\n", g_failures, g_checks);
		return (EXIT_FAILURE);
	}

	printf("[" GREEN "PASS" RESET "] %u controls passed\n", g_checks);
	return (EXIT_SUCCESS);
}
