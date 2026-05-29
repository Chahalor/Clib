/**
 * 
*/
/* ----| Headers    |----- */
	/* Standard */
#include <string.h>

	/* Internal */
#include "manager.h"

	/* External */
#include "interface/args.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

void	_extract_init(
	Config *const				config,
	t_args_output_parser *const	sub_out
)
{
	size_t					n = 0;
	char					*init_cache_dir;
	t_args_output_option	*init_url = args_get_option(sub_out, "remote-url");

	init_cache_dir = args_get_param(sub_out, "cache-dir", &n);
	if (init_cache_dir)
		config->consts.path_cache_dir = init_cache_dir;

	if (init_url)
		config->consts.url_git = args_get_param(init_url, "url", &n);
}

void	_extract_list(
	Config *const				config,
	t_args_output_parser *const	out
)
{
	size_t							n;
	typeof(config->sub.list) *const	d = &config->sub.list;
	t_args_output_option *			opt_dest;
	t_args_output_option *			opt_installed;

	opt_dest = args_get_option(out, "deps");
	if (opt_dest)
	{
		d->options.deps = true;
		d->deps_dir = args_get_param(opt_dest, "path", &n);
		if (n > 1)
			d->deps_dir = ((char **)d->deps_dir)[0];
	}
	d->options.version  = args_get_option(out, "version") != NULL;
	d->options.stats = args_get_option(out, "stats") != NULL;
	opt_installed = args_get_option(out, "installed");
	if (opt_installed)
	{
		d->options.installed = true;
		config->dest = args_get_param(opt_installed, "path", &n);
		if (n > 1)
			config->dest = ((char **)config->dest)[0];
	}
	d->options.config = args_get_option(out, "config") != NULL;
}

/* ----| Public     |----- */

t_args_parser	*setup_args(void)
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
	args_add_param(list, "target", "target of the lsiting action", args_param_specs_nargs, param_type_str);
	args_parser_add_option(list, "deps", 'd', "show the dependencies of each module");
	args_parser_add_option(list, "version", 'v', "show the version of each modules");
	args_parser_add_option(list, "stats", 's', "show many stats about each module, like they size, number of file, ...");
	t_args_option	*opt_list_installed = args_parser_add_option(list, "installed", 'i', "show which module are currently installed in the pwd. By default will check ./clib or ./Clib. By using --installed=<path> it will use this path insted");
	args_add_param(opt_list_installed, "path", NULL, 0, param_type_file);
	args_parser_add_option(list, "config", 'c', "show information about the current configuration used and the cache");

	/* Sub-parser help */
	help = args_parser_add_sub(result, "help", "display the app help");

	return (result);
}

int	arsg_extract(
	t_args_output	*const	out,
	Config			*const	config
)
{
	t_args_output_option	*output = args_get_option(out, "output");
	t_args_output_option	*file = args_get_option(out, "config");
	t_args_output_option	*cache_dir = args_get_option(out, "cache-dir");
	t_args_output_option	*remote_url = args_get_option(out, "remote-url");
	size_t					n = 0;
	const char				*sub = NULL;
	t_args_output_parser	*sub_out = NULL;

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
	sub_out = args_get_sub_output(out);
	if (!sub)
		config->sub.name = UNKNOW;

	else if (!strcmp("setup", sub))
		config->sub.name = SETUP;
	else if (!strcmp("update", sub))
		config->sub.name = UPDATE;
	else if (!strcmp("init", sub))
	{
		_extract_init(config, sub_out);
		config->sub.name = INIT;
	}
	else if (!strcmp("list", sub))
	{
		_extract_list(config, sub_out);
		config->sub.name = LIST;
	}
	else if (!strcmp("help", sub))
	{
		config->sub.name = HELP;
		config->cli.help = true;
	}
	else
		config->sub.name = UNKNOW;

	return (config->sub.name == UNKNOW);
}
