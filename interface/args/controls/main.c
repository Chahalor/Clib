/**
 * @file main.c
 * @brief Controls for the public API of interface/args.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../args.h"
#include "../_internal_/_args.h"
#include "../../../memory.h"
#include "../../../standards/formating.h"

static unsigned int	g_checks = 0;
static unsigned int	g_failures = 0;

#define CHECK(_cond, _msg) \
	do \
	{ \
		g_checks++; \
		if ((_cond)) \
			printf("[" GREEN "PASS" RESET "] %s\n", _msg); \
		else \
		{ \
			g_failures++; \
			fprintf(stderr, "[" RED "FAIL" RESET "] %s:%d: %s\n", __FILE__, __LINE__, _msg); \
		} \
	} while (0)

static void	_free_param_result(
	void *value
)
{
	if (value)
		mem_free(value);
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
	if (!args_add_param(parser, "input", "input file", args_param_specs_require, param_type_file))
		goto fail;
	if (!args_add_param(parser, "extras", "extra args", args_param_specs_nargs, param_type_str))
		goto fail;

	sub = args_parser_add_sub(parser, "pull", "pull command");
	if (!sub)
		goto fail;
	if (!args_add_param(sub, "repo", "repository", args_param_specs_require, param_type_str))
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
	if (!args_add_param(parser, "input", "input file", args_param_specs_require, param_type_file))
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
	if (!args_add_param(parser, "first", "first positional", args_param_specs_require, param_type_str))
		goto fail;

	return (parser);
fail:
	args_parser_free(parser);
	return (NULL);
}

static void	_test_ensure_tester(void)
{
	CHECK(false, "should be a failed");
	g_checks--;
	g_failures--;
}

static void	_test_public_api_basic(void)
{
	t_args_parser	*parser;
	t_args_option	*opt;
	t_args_param	*param;
	size_t			n;
	const char		*argv[] = {"args-controls"};

	CHECK(args_parse(NULL, 1, argv) == NULL, "args_parse(NULL, ...) must fail");
	CHECK(args_error(NULL) == args_error_internal, "args_error(NULL) must be internal");
	CHECK(args_error_str(args_error_unknown_option) != NULL, "args_error_str must return text");
	CHECK(args_parser_set_desc(NULL, "x") == error_invalid_arg, "parser set desc null guard");
	CHECK(args_param_set_desc(NULL, "x") == error_invalid_arg, "param set desc null guard");
	CHECK(args_option_set_desc(NULL, "x") == error_invalid_arg, "option set desc null guard");
	CHECK(args_has_option(NULL, "x") == error_invalid_arg, "has_option null guard");
	CHECK(args_has_param(NULL, "x") == error_invalid_arg, "has_param null guard");
	CHECK(args_has_sub(NULL, "x") == error_invalid_arg, "has_sub null guard");
	CHECK(args_option_has_param(NULL, "x") == error_invalid_arg, "option_has_param null guard");

	n = 0;
	CHECK(args_get_option((t_args_output *)NULL, "x") == NULL, "get_option null output guard");
	CHECK(args_get_param((t_args_output *)NULL, "x", &n) == NULL, "get_param null output guard");
	CHECK(args_get_option((t_args_output_parser *)NULL, "x") == NULL,
		"sub get_option null output guard");
	CHECK(args_get_param((t_args_output_parser *)NULL, "x", &n) == NULL,
		"sub get_param null output guard");

	parser = args_parser_new();
	CHECK(parser != NULL, "args_parser_new should allocate parser");
	if (!parser)
		return ;

	opt = args_parser_add_option(parser, "sample", 's', "sample option");
	param = args_add_param(parser, "input", "input param", args_param_specs_require, param_type_str);
	CHECK(opt != NULL, "add option should succeed");
	CHECK(param != NULL, "add parser param should succeed");
	if (opt)
	{
		CHECK(args_option_add_param(opt, "value", "sample value",
				args_param_specs_require, param_type_str) != NULL,
			"add option param should succeed");
	}

	CHECK(args_set_description(parser, "updated parser") == error_none, "parser desc should be editable");
	CHECK(strcmp("updated parser", parser->desc) == 0, "checking the new parser description");
	if (param)
	{
		CHECK(args_set_description(param, "updated param") == error_none, "param desc should be editable");
		CHECK(strcmp("updated param", param->desc) == 0, "checking the new param description");
	}
	if (opt)
	{
		CHECK(args_set_description(opt, "updated option") == error_none, "option desc should be editable");
		CHECK(strcmp("updated option", opt->desc) == 0, "checking the new option description");
	}

	args_parser_free(parser);
}

static void	_test_root_parse_output(void)
{
	t_args_parser			*parser;
	t_args_output			*out;
	t_args_output_option	*opt;
	size_t					count;
	char					*input;
	char					**extras;
	const char				*argv[] = {
		"args-controls",
		"--config", "settings.json",
		"-v",
		"input.txt", "extra1", "extra2"
	};
	const int				argc = (int)(sizeof(argv) / sizeof(argv[0]));

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

	CHECK(args_error(out) == args_error_none, "valid root parse should have no error");
	CHECK(args_has_option(out, "config") == 1, "config option should be present");
	CHECK(args_has_option(out, "-v") == 1, "verbose option should be present");
	CHECK(args_has_param(out, "input") == 1, "input param should be present");
	CHECK(args_has_param(out, "extras") == 1, "extras param should be present");
	CHECK(args_has_sub(out, "pull") == 0, "no subcommand should be selected");
	CHECK(args_active_subcommand(out) == NULL, "active subcommand should be null");
	CHECK(args_get_sub_output(out) == NULL, "sub output should be null");

	opt = args_get_option(out, "config");
	CHECK(opt != NULL, "config option extraction should succeed");
	if (opt)
		CHECK(args_option_has_param(opt, "path") == 1, "config option should expose path param");

	opt = args_get_option(out, "verbose");
	CHECK(opt != NULL, "verbose option extraction should succeed");
	if (opt)
		CHECK(args_option_has_param(opt, "path") == 0,
			"verbose option should not expose config param");

	count = 0;
	input = (char *)args_get_param(out, "input", &count);
	CHECK(input != NULL && count == 1, "input should contain exactly one value");
	if (input)
		CHECK(!strcmp(input, "input.txt"), "input value should match");
	_free_param_result(input);

	count = 0;
	extras = (char **)args_get_param(out, "extras", &count);
	CHECK(extras != NULL && count == 2, "extras should contain two values");
	if (extras)
	{
		CHECK(!strcmp(extras[0], "extra1"), "extras first value should match");
		CHECK(!strcmp(extras[1], "extra2"), "extras second value should match");
	}
	_free_param_result(extras);

	opt = args_get_option(out, "missing");
	CHECK(opt == NULL, "missing option should not be found");

	count = 0;
	CHECK(args_get_param(out, "missing", &count) == NULL && count == 0,
		"missing param should return null and count 0");

	args_output_free(out);
	args_parser_free(parser);
}

static void	_test_subcommand_output(void)
{
	t_args_parser			*parser;
	t_args_output			*out;
	t_args_output_parser	*sub;
	t_args_output_option	*opt;
	size_t					count;
	char					*repo;
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

	CHECK(args_error(out) == args_error_none, "valid sub parse should have no error");
	CHECK(args_has_option(out, "config") == 1, "root config should still be present");
	CHECK(args_has_sub(out, "pull") == 1, "pull subcommand should be present");
	CHECK(args_active_subcommand(out) && !strcmp(args_active_subcommand(out), "pull"),
		"active subcommand should be pull");

	sub = args_get_sub_output(out);
	CHECK(sub != NULL, "sub output should be returned");
	if (!sub)
	{
		args_output_free(out);
		args_parser_free(parser);
		return ;
	}

	count = 0;
	repo = (char *)args_get_param(sub, "repo", &count);
	CHECK(repo != NULL && count == 1, "repo should contain one value");
	if (repo)
		CHECK(!strcmp(repo, "origin/main"), "repo value should match");
	_free_param_result(repo);

	opt = args_get_option(sub, "todo");
	CHECK(opt != NULL, "sub todo option should be present");
	if (opt)
		CHECK(args_option_has_param(opt, "file") == 1,
			"sub todo option should expose file param");

	opt = args_get_option(sub, "force");
	CHECK(opt != NULL, "sub force option should be present");
	if (opt)
		CHECK(args_option_has_param(opt, "file") == 0,
			"sub force option should not expose file param");

	opt = args_get_option(sub, "config");
	CHECK(opt == NULL, "sub should not expose root config option");

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

	out = args_parse(parser,
			(int)(sizeof(argv_unknown) / sizeof(argv_unknown[0])), argv_unknown);
	CHECK(out != NULL, "unknown option parse should return output");
	if (out)
	{
		CHECK(args_error(out) == args_error_unknown_option,
			"unknown option should set args_error_unknown_option");
		args_output_free(out);
	}

	out = args_parse(parser,
			(int)(sizeof(argv_missing_opt_value) / sizeof(argv_missing_opt_value[0])),
			argv_missing_opt_value);
	CHECK(out != NULL, "missing option value parse should return output");
	if (out)
	{
		CHECK(args_error(out) == args_error_missing_param,
			"missing option value should set args_error_missing_param");
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
	t_args_parser			*parser;
	t_args_output			*out;
	t_args_output_option	*opt;
	size_t					count;
	char					*first;
	const char				*argv[] = {
		"args-controls",
		"-ab",
		"--config", "conf.yml",
		"--",
		"-c"
	};
	const int				argc = (int)(sizeof(argv) / sizeof(argv[0]));

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

	opt = args_get_option(out, "config");
	CHECK(opt != NULL, "config option extraction should succeed");
	if (opt)
		CHECK(args_option_has_param(opt, "path") == 1,
			"config option should expose path param");

	count = 0;
	first = (char *)args_get_param(out, "first", &count);
	CHECK(first != NULL && count == 1, "first positional should contain one value");
	if (first)
		CHECK(!strcmp(first, "-c"), "delimiter should keep '-c' as a positional value");
	_free_param_result(first);

	args_output_free(out);
	args_parser_free(parser);
}

int	main(void)
{
	_test_ensure_tester();
	_test_public_api_basic();
	_test_root_parse_output();
	_test_subcommand_output();
	_test_error_paths();
	_test_basic_parsing_controls();

	if (g_failures)
	{
		fprintf(stderr, "\n[" RED "FAIL" RESET "] %u/%u controls failed\n", g_failures, g_checks);
		return (EXIT_FAILURE);
	}

	printf("[" GREEN "PASS" RESET "] %u controls passed\n", g_checks);
	return (EXIT_SUCCESS);
}
