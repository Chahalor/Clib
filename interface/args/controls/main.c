/**
 * @file main.c
 * @brief Controls for the public API of interface/args.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

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

static t_args_parser	*_build_inline_option_parser(void)
{
	t_args_parser	*parser;
	t_args_option	*opt;

	parser = args_parser_new();
	if (!parser)
		return (NULL);
	opt = args_parser_add_option(parser, "file", 'f', "file path");
	if (!opt || !args_option_add_param(opt, "file", "path to file",
			args_param_specs_require, param_type_file))
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

static void	_test_inline_option_value_controls(void)
{
	t_args_parser			*parser;
	t_args_output			*out;
	t_args_output_option	*opt;
	size_t					count;
	char					*file;
	const char				*argv_inline_ok[] = {"args-controls", "--file=bob"};
	const char				*argv_inline_short_invalid[] = {"args-controls", "-f=bob"};
	const char				*argv_missing_inline_value[] = {"args-controls", "--file="};
	const char				*argv_missing_value[] = {"args-controls", "--file"};

	parser = _build_inline_option_parser();
	CHECK(parser != NULL, "inline option parser setup should succeed");
	if (!parser)
		return ;

	out = args_parse(parser,
			(int)(sizeof(argv_inline_ok) / sizeof(argv_inline_ok[0])),
			argv_inline_ok);
	CHECK(out != NULL, "inline long option parse should return output");
	if (out)
	{
		CHECK(args_error(out) == args_error_none,
			"inline long option value should be parsed without error");
		CHECK(args_has_option(out, "file") == 1, "inline long option should be present");
		opt = args_get_option(out, "file");
		CHECK(opt != NULL, "inline long option extraction should succeed");
		if (opt)
		{
			CHECK(args_option_has_param(opt, "file") == 1,
				"inline long option should expose its required parameter");
			count = 0;
			file = (char *)args_get_param(opt, "file", &count);
			CHECK(file != NULL && count == 1,
				"inline long option should contain exactly one value");
			if (file)
				CHECK(!strcmp(file, "bob"), "inline long option value should match");
			_free_param_result(file);
		}
		CHECK(args_has_param(out, "file") == 0,
			"inline option value must not be stored as a positional parameter");
		args_output_free(out);
	}

	out = args_parse(parser,
			(int)(sizeof(argv_inline_short_invalid) / sizeof(argv_inline_short_invalid[0])),
			argv_inline_short_invalid);
	CHECK(out != NULL, "inline short option syntax parse should return output");
	if (out)
	{
		CHECK(args_error(out) == args_error_missing_param,
			"inline short option syntax should be rejected as missing parameter");
		args_output_free(out);
	}

	out = args_parse(parser,
			(int)(sizeof(argv_missing_inline_value) / sizeof(argv_missing_inline_value[0])),
			argv_missing_inline_value);
	CHECK(out != NULL, "empty inline value parse should return output");
	if (out)
	{
		CHECK(args_error(out) == args_error_missing_param,
			"empty inline value should report missing required parameter");
		args_output_free(out);
	}

	out = args_parse(parser,
			(int)(sizeof(argv_missing_value) / sizeof(argv_missing_value[0])),
			argv_missing_value);
	CHECK(out != NULL, "missing option value parse should return output");
	if (out)
	{
		CHECK(args_error(out) == args_error_missing_param,
			"missing option value should report missing required parameter");
		args_output_free(out);
	}

	args_parser_free(parser);
}

static void	_test_manual(
	const int argc,
	const char *argv[]
)
{
	t_args_parser	*parser = args_parser_new();

	CHECK(parser != NULL, "parser allocation");
	if (unlikely(!parser))
		return ;

	t_args_option	*file = args_parser_add_option(parser, "file", 'f', "a file");
	CHECK(file != NULL, "option alloc");
	if (unlikely(!file))
	{
		args_parser_free(parser);
		return ;
	}
	args_add_param(file, "file", "a file", args_param_specs_require, param_type_file);

	t_args_output	*out = args_parse(parser, argc, argv);

	if (args_error(out))
	{
		fprintf(stderr, "parsing error: %s\n", args_error_str(args_error(out)));
		args_parser_free(parser);
		args_output_free(out);
		return ;
	}

	if (args_has_option(out, "file"))
	{
		t_args_output_option	*out_file = args_get_option(out, "file");
		size_t					n = 0;
		char					*data = args_get_param(out_file, "file", &n);

		if (!n)
			fprintf(stderr, "--file is empty\n");
		else
			fprintf(stderr, "--file=%s\n", data);
	}

	args_parser_free(parser);
	args_output_free(out);
}

static int	_run_non_short_pipeline_case(
	const int argc,
	const char *argv[],
	const bool expect_config,
	const bool expect_dry_run,
	const bool expect_bob,
	const char *const expect_config_value
)
{
	t_args_parser			*parser = NULL;
	t_args_option			*config = NULL;
	t_args_output			*out = NULL;
	t_args_output_option	*opt = NULL;
	char					*value = NULL;
	size_t					n = 0;
	int						result = 42;

	parser = args_parser_new();
	if (!parser)
		return (10);
	config = args_parser_add_option(parser, "config", 0, "config file");
	if (!config)
	{
		result = 11;
		goto cleanup_parser;
	}
	if (!args_option_add_param(config, "file", "config path", args_param_specs_require, param_type_file))
	{
		result = 12;
		goto cleanup_parser;
	}
	if (!args_parser_add_option(parser, "bob", 'b', "bob option"))
	{
		result = 13;
		goto cleanup_parser;
	}
	if (!args_parser_add_option(parser, "dry-run", 0, "dry run mode"))
	{
		result = 14;
		goto cleanup_parser;
	}
	out = args_parse(parser, argc, argv);
	if (!out)
	{
		result = 15;
		goto cleanup_parser;
	}
	if (args_error(out) != args_error_none)
	{
		result = 16;
		goto cleanup;
	}
	if ((args_has_option(out, "config") == true) != expect_config)
	{
		result = 17;
		goto cleanup;
	}
	if ((args_has_option(out, "--config") == true) != expect_config)
	{
		result = 18;
		goto cleanup;
	}
	if ((args_has_option(out, "dry-run") == true) != expect_dry_run)
	{
		result = 19;
		goto cleanup;
	}
	if ((args_has_option(out, "--dry-run") == true) != expect_dry_run)
	{
		result = 20;
		goto cleanup;
	}
	if ((args_has_option(out, "bob") == true) != expect_bob)
	{
		result = 21;
		goto cleanup;
	}
	if ((args_has_option(out, "b") == true) != expect_bob)
	{
		result = 22;
		goto cleanup;
	}
	opt = args_get_option(out, "config");
	if ((opt != NULL) != expect_config)
	{
		result = 23;
		goto cleanup;
	}
	if (expect_config && expect_config_value)
	{
		if (args_option_has_param(opt, "file") != true)
		{
			result = 24;
			goto cleanup;
		}
		value = args_get_param(opt, "file", &n);
		if (!value || n != 1 || strcmp(value, expect_config_value))
		{
			result = 25;
			goto cleanup;
		}
		mem_free(value);
		value = NULL;
	}
	opt = args_get_option(out, "dry-run");
	if ((opt != NULL) != expect_dry_run)
	{
		result = 26;
		goto cleanup;
	}
	opt = args_get_option(out, "bob");
	if ((opt != NULL) != expect_bob)
	{
		result = 27;
		goto cleanup;
	}
	opt = args_get_option(out, "-b");
	if ((opt != NULL) != expect_bob)
	{
		result = 28;
		goto cleanup;
	}

cleanup:
	if (value)
		mem_free(value);
	args_output_free(out);
cleanup_parser:
	args_parser_free(parser);
	return (result);
}

static void	_test_non_short_pipeline_case(
	const char *const label,
	const int argc,
	const char *argv[],
	const bool expect_config,
	const bool expect_dry_run,
	const bool expect_bob,
	const char *const expect_config_value
)
{
	pid_t	pid;
	int		status = 0;

	fflush(NULL);
	pid = fork();
	CHECK(pid >= 0, "non-short pipeline fork should succeed");
	if (pid < 0)
		return ;
	if (pid == 0)
	{
		if (!freopen("/dev/null", "w", stdout))
			_Exit(90);
		if (!freopen("/dev/null", "w", stderr))
			_Exit(91);
		_Exit(_run_non_short_pipeline_case(argc, argv, expect_config, expect_dry_run, expect_bob, expect_config_value));
	}

	CHECK(waitpid(pid, &status, 0) == pid, "non-short pipeline wait should succeed");
	if (WIFEXITED(status))
		CHECK(WEXITSTATUS(status) == 42, label);	// if the exit code == that mean the exit is from the help
	else
		CHECK(false, label);
}

static void	_test_non_short_options(void)
{
	const char				*argv_dry_only[3] = {"args-controls", "--dry-run", NULL};
	const char				*argv_mixed[6] = {"args-controls", "--dry-run", "--config", "settings.json", "-b", NULL};
	_t_args_output_option	opt_bob = {
		.short_name = 'b',
		.nb_call = 1,
		.error = error_none,
		.long_name = "bob",
		.params = NULL,
		.next = NULL
	};
	_t_args_output_option	opt_dry_run = {
		.short_name = 0,
		.nb_call = 1,
		.error = error_none,
		.long_name = "dry-run",
		.params = NULL,
		.next = &opt_bob
	};
	_t_args_output_parser	root = {
		.name = "args-controls",
		.options = &opt_dry_run,
		.params = NULL,
		.sub = NULL,
		.next = NULL
	};
	_t_args_output			out = {
		.error = error_none,
		.root = &root
	};

	CHECK(args_has_option(&out, "dry-run") == true,
		"has option --dry-run should be true");
	CHECK(args_has_option(&out, "--dry-run") == true,
		"has option --dry-run with explicit prefix should be true");
	CHECK(args_has_option(&out, "config") == false,
		"has option --config should stay false when only --dry-run is present");
	CHECK(args_has_option(&out, "--config") == false,
		"has option --config with explicit prefix should stay false");
	CHECK(args_has_option(&out, "bob") == true,
		"has option --bob should be true");
	CHECK(args_has_option(&out, "b") == true,
		"has option -b should be true");
	CHECK(args_has_option(&out, "c") == false,
		"has option -c should be false");
	_test_non_short_pipeline_case(
		"pipeline: --dry-run should not imply --config",
		2,
		argv_dry_only,
		false,
		true,
		false,
		NULL
	);
	_test_non_short_pipeline_case(
		"pipeline: setup/parse/extract should keep mixed options isolated",
		5,
		argv_mixed,
		true,
		true,
		true,
		"settings.json"
	);
}

static void	_test_json_parser(void)
{
	const char			*definition =
		"{"
		"\"name\":\"json-demo\","
		"\"description\":\"parser loaded from JSON\","
		"\"parameters\":[{"
			"\"name\":\"input\",\"dsc\":\"input file\","
			"\"type\":\"file\",\"specs\":[\"requiered\"]}],"
		"\"options\":["
			"{\"long\":\"config\",\"short\":\"c\","
			"\"description\":\"configuration file\",\"params\":[{"
				"\"name\":\"path\",\"type\":\"file\","
				"\"specs\":[\"require\"]}]},"
			"{\"long-name\":\"verbose\",\"short-name\":\"v\"}],"
		"\"sub-parser\":[{"
			"\"name\":\"run\",\"desc\":\"run a target\","
			"\"params\":[{\"name\":\"target\",\"type\":\"string\","
				"\"specs\":[\"required\"]}],"
			"\"opts\":[{\"long\":\"force\",\"short\":\"f\"}]}]"
		"}";
	const char			*argv[] = {
		"json-demo", "--config", "settings.json", "source.txt",
		"run", "project", "--force"
	};
	JSON				*json;
	t_args_parser		*parser;
	t_args_parser		*sub;
	t_args_option		*config;
	t_args_param		*input;
	t_args_output		*out;

	json = json_load_str(definition);
	CHECK(json != NULL, "JSON args definition should load");
	if (!json)
		return ;
	parser = args_parser_json(json);
	CHECK(parser != NULL, "JSON args definition should build a parser");
	if (!parser)
	{
		json_unload(json);
		return ;
	}
	config = _args_parser_get_option(parser, "config");
	input = _args_parser_get_param(parser, "input");
	sub = parser->sub_parsers;
	CHECK(parser->name && !strcmp(parser->name, "json-demo"),
		"JSON parser name should be copied");
	CHECK(config && config->short_name == 'c' && config->params,
		"JSON option and its parameter should be built");
	CHECK(input && input->desc && !strcmp(input->desc, "input file")
		&& input->type == param_type_file
		&& (input->specs & args_param_specs_require),
		"JSON parameter type and required spec should be decoded");
	CHECK(sub && sub->name && !strcmp(sub->name, "run")
		&& _args_parser_get_option(sub, "force"),
		"JSON subcommand and nested option should be built");

	puts("\nJSON parser generated successfully:");
	_dump_parser(parser);
	putchar('\n');
	out = args_parse(parser, (int)(sizeof(argv) / sizeof(argv[0])), argv);
	CHECK(out && args_error(out) == args_error_none,
		"parser generated from JSON should parse argv");
	CHECK(out && args_has_option(out, "config"),
		"JSON-generated config option should be present");
	CHECK(out && args_has_sub(out, "run"),
		"JSON-generated run subcommand should be active");
	args_output_free(out);
	args_parser_free(parser);
	json_unload(json);

	json = json_load_str("{\"params\":\"not-an-array\"}");
	parser = args_parser_json(json);
	CHECK(parser == NULL, "invalid JSON args field types should be rejected");
	args_parser_free(parser);
	json_unload(json);
}

int	main(int argc, const char *argv[])
{
	_test_public_api_basic();
	_test_root_parse_output();
	_test_subcommand_output();
	_test_error_paths();
	_test_basic_parsing_controls();
	_test_inline_option_value_controls();
	_test_non_short_options();
	_test_json_parser();

	if (argc > 1)
		_test_manual(argc, argv);

	if (g_failures)
	{
		fprintf(stderr, "\n[" RED "FAIL" RESET "] %u/%u controls failed\n", g_failures, g_checks);
		return (EXIT_FAILURE);
	}

	printf("[" GREEN "PASS" RESET "] %u controls passed\n", g_checks);
	return (EXIT_SUCCESS);
}
