/**
 * 
*/
/* ----| Headers    |----- */
	/* Standard */
#include <errno.h>
#include <string.h>
#include <stdlib.h>

	/* Internal */
#include "types.h"
#include "manager.h"

	/* External */
#include "files/toml.h"
#include "interface/args.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

t_args_parser	*_setup_args(void)
{
	t_args_parser	*result;
	t_args_parser	*init;
	t_args_parser	*setup;
	t_args_parser	*update;
	t_args_parser	*list;
	t_args_parser	*help;

	result = args_parser_new();
	if (unlikely(!result))
		return (NULL);

	/* Main parser options */
	t_args_option	*output = args_parser_add_option(result, "output", 'o', "destination of the action");
	args_add_param(output, "path", NULL, args_param_specs_require, param_type_file);

	t_args_option	*file = args_parser_add_option(result, "config", 'c', "config file that will be used for this action");
	args_add_param(file, "path", NULL, args_param_specs_require, param_type_file);

	t_args_option	*cache_dir = args_parser_add_option(result, "cache-dir", 0, "path to the cache dir used for this action");
	args_add_param(cache_dir, "path", NULL, args_param_specs_require, param_type_file);

	t_args_option	*remote_url = args_parser_add_option(result, "remote-url", 0, "remote url used for this action");
	args_add_param(remote_url, "url", NULL, args_param_specs_require, param_type_str);

	args_parser_add_option(result, "verbose", 'v', "enable the verbose mode for the app");

	/* Sub-parser setup */
	setup = args_parser_add_sub(result, "setup", "setup the module in the targeted dir");
	args_add_param(setup, "target", "the requested module to be setup", args_param_specs_nargs | args_param_specs_require, param_type_str);
	args_parser_add_option(setup, "init", 0, "if the lib is not init in the environement, init it");

	/* Sub-parser init */
	init = args_parser_add_sub(result, "init", "init the lib for the full session");
	args_add_param(init, "cache-dir", "The dir used to store the lib cache", 0, param_type_file);
	t_args_option	*init_url = args_parser_add_option(init, "remote-url", 0, "the url to the cache remote git");
	args_add_param(init_url, "url", NULL, args_param_specs_require, param_type_str);

	/* Sub-parser update */
	update = args_parser_add_sub(result, "update", "the module to they latest version");
	args_add_param(update, "target", "the requested module to be updated", args_param_specs_nargs | args_param_specs_require, param_type_str);

	/* Sub-parser list */
	list = args_parser_add_sub(result, "list", "List all the avaible module in the current system");

	/* Sub-parser help */
	help = args_parser_add_sub(result, "help", "display the app help");

	return (result);
}

int	_arsg_extract(
	t_args_output	*const	out,
	Config			*const	config
)
{
	t_args_output_option	*output = args_get_option(out, "output");
	t_args_output_option	*file = args_get_option(out, "config");
	t_args_output_option	*cache_dir = args_get_option(out, "cache-dir");
	t_args_output_option	*remote_url = args_get_option(out, "remote-url");
	t_args_output_parser	*sub_out = NULL;
	size_t					n = 0;
	const char				*sub = NULL;

	if (output)
		config->dest = args_get_param(output, "path", &n);

	if (!config->dest)
		config->dest = ".";

	if (file)
		config->config_file = args_get_param(file, "path", &n);

	config->consts.path_config_file = config->config_file;

	if (cache_dir)
		config->consts.path_cache_dir = args_get_param(cache_dir, "path", &n);

	if (remote_url)
		config->consts.url_git = args_get_param(remote_url, "url", &n);

	config->cli.verbose = args_has_option(out, "verbose");
	config->cli.help    = args_has_option(out, "help");

	sub = args_active_subcommand(out);
	if (!sub)
		config->sub = UNKNOW;
	else if (!strcmp("setup", sub))
		config->sub = SETUP;
	else if (!strcmp("update", sub))
		config->sub = UPDATE;
	else if (!strcmp("init", sub))
		config->sub = INIT;
	else if (!strcmp("list", sub))
		config->sub = LIST;
	else if (!strcmp("help", sub))
	{
		config->sub = HELP;
		config->cli.help = true;
	}
	else
		config->sub = UNKNOW;

	sub_out = args_get_sub_output(out);
	if (sub_out && config->sub == INIT)
	{
		t_args_output_option	*init_url = args_get_option(sub_out, "remote-url");
		char					*init_cache_dir;

		init_cache_dir = args_get_param(sub_out, "cache-dir", &n);
		if (init_cache_dir)
			config->consts.path_cache_dir = init_cache_dir;

		if (init_url)
			config->consts.url_git = args_get_param(init_url, "url", &n);
	}

	return (config->sub == UNKNOW);
}

/* ----| Public     |----- */

int main(int argc, char const *argv[])
{
	int				exit_status = EXIT_SUCCESS;
	Config			config = {0};
	t_args_parser	*parser = _setup_args();
	t_args_output	*output = NULL;
	int				err = 0;

	if (unlikely(!parser))
	{
		perror("parser setup");
		return (errno);
	}

	output = args_parse(parser, argc, argv);
	if (unlikely(!output))
	{
		perror("args parsing");
		return (errno);
	}

	if (unlikely(args_error(output)))
	{
		fprintf(stderr, "invalid args during parsing: %s\n", args_error_str(args_error(output)));
		return (EINVAL);
	}

	if (unlikely(_arsg_extract(output, &config)))
	{
		fprintf(stderr, "invalid args found\n");
		args_show_help(parser, EINVAL);
	}
	else if (config.cli.help)
		args_show_help(parser, EXIT_SUCCESS);

	switch (config.sub)
	{
	case INIT:
	{
		exit_status = init_all(&config);
		break;
	}
	case SETUP:
	{
		err = setup_setup(&config, output);
		if (unlikely(err))
			return (err);

		err = config_load(&config, config.consts.path_config_file, config.cli.init);
		if (unlikely(err))
			return (err);

		err = modules_load(&config, config.consts.path_modules_file);
		if (unlikely(err))
			return (err);

		exit_status = setup(&config);
		break;
	}
	case UPDATE:
		fprintf(stderr, "Not implemented yet\n");
		exit_status = ENOSYS;
		break;
	case LIST:
		fprintf(stderr, "Not implemented yet\n");
		exit_status = ENOSYS;
		break;
	default:
		args_show_help(parser, EINVAL);
		break;
	}

	args_parser_free(parser);
	args_output_free(output);
	return (exit_status);
}
