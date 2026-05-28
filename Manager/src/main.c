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
	t_args_parser	*setup;
	t_args_parser	*update;
	t_args_parser	*export;

	result = args_parser_new();
	if (unlikely(!result))
		return (NULL);

	t_args_option	*output = args_parser_add_option(result, "output", 'o', "destination of the action");
	args_add_param(output, "path", NULL, args_param_specs_require, param_type_file);

	t_args_option	*file = args_parser_add_option(result, "config", 'c', "config file that will be used for this action");
	args_add_param(file, "path", NULL, args_param_specs_require, param_type_file);

	setup = args_parser_add_sub(result, "setup", "setup the module in the targeted dir");
	args_add_param(setup, "target", "the requested module to be setup", args_param_specs_nargs | args_param_specs_require, param_type_str);

	update = args_parser_add_sub(result, "update", "the module to they latest version");
	args_add_param(update, "target", "the requested module to be updated", args_param_specs_nargs | args_param_specs_require, param_type_str);

	export = args_parser_add_sub(result, "export", "export the module configuration");
	args_add_param(export, "dir", "the module directory to be used", 0, param_type_str);

	return (result);
}

int	_arsg_extract(
	t_args_output	*const	out,
	Config			*const	config
)
{
	t_args_output_option	*output = args_get_option(out, "output");
	t_args_output_option	*file = args_get_option(out, "config");
	size_t					n = 0;
	const char				*sub = NULL;

	if (output)
		config->dest = args_get_param(output, "path", &n);

	if (!config->dest)
		config->dest = ".";

	if (file)
		config->config_file = args_get_param(file, "path", &n);

	if (!config->config_file)
		config->config_file = ".clib";

	sub = args_active_subcommand(out);
	if (!sub)
		config->sub = UNKNOW;
	else if (!strcmp("setup", sub))
		config->sub = SETUP;
	else if (!strcmp("update", sub))
		config->sub = UPDATE;
	else if (!strcmp("export", sub))
		config->sub = EXPORT;
	else
		config->sub = UNKNOW;

	return (config->sub == UNKNOW);
}

int	_toml_extract(
	Config *const config
)
{
	struct s_conf_file *const	f = &config->conf;

	f->name = toml_get(f->toml, "name");
	if (!f->name)
		f->name = toml_get(f->toml, "Name");

	f->version = toml_get(f->toml, "version");
	if (!f->version)
		f->version = toml_get(f->toml, "version");

	f->remote_url = toml_get(f->toml, "url");
	if (!f->remote_url)
	{
		f->remote_url = toml_get(f->toml, "Url");
		if (unlikely(!f->remote_url))
		{
			fprintf(stderr, "Error: missing `url` field in the config file\n");
			return (EINVAL);
		}
	}

	toml_foreach(array, f->toml)
	{
		t_module	*new = NULL;
		TOML		*node;

		if (toml_get_type(array) != toml_table)
			continue ;

		new = module_new();
		if (unlikely(!new))
			return (1);

		new->name = toml_get(array, "name");
		new->path = toml_get(array, "path");
		new->public_header = toml_get(array, "publicHeaders");
		new->private_header = toml_get(array, "privateHeaders");
		node = toml_get(array, "dependencies");
		if (node && toml_len(node))
		{
			toml_foreach(dep, node)
			{
				array_append(&new->dependencies, dep);
			}
		}

		node = toml_get(array, "tags");
		if (node && toml_len(node))
		{
			toml_foreach(dep, node)
			{
				array_append(&new->dependencies, dep);
			}
		}

		node = toml_get(array, "controls");
		if (node && toml_len(node))
		{
			toml_foreach(dep, node)
			{
				array_append(&new->dependencies, dep);
			}
		}

		node = toml_get(array, "defines");
		if (node && toml_len(node))
		{
			toml_foreach(dep, node)
			{
				array_append(&new->dependencies, dep);
			}
		}

		array_append(&config->conf.modules, new);
	}
	return (error_none);
}

/* ----| Public     |----- */

int main(int argc, char const *argv[])
{
	int				exit_status = EXIT_SUCCESS;
	Config			config = {0};
	t_args_parser	*parser = _setup_args();
	t_args_output	*output = NULL;

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
		fprintf(stderr, "invalid args found\n");

	config.conf.toml = toml_load_file(config.config_file);
	if (unlikely(!config.conf.toml))
	{
		toml_error_dump(stderr);
		return (toml_errno());
	}

	if (unlikely(_toml_extract(&config)))
		return (EXIT_FAILURE);

	toml_dump(config.conf.toml, stdout, 4);

	switch (config.sub)
	{
	case SETUP:
	{
		t_args_output_parser	*sub = args_get_sub_output(output);
		size_t					n;
		char					*dest = args_get_param(sub, "target", &n);

		config.dest = dest;
		setup(&config);
		break;
	}
	case UPDATE:
		fprintf(stderr, "Not implemented yet\n");
		break;
	case EXPORT:
		fprintf(stderr, "Not implemented yet\n");
		break;
	default:
		args_show_help(parser, EINVAL);
		break;
	}

	args_parser_free(parser);
	args_output_free(output);
	return (exit_status);
}
