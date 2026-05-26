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
	args_add_param(setup, "target", "the requested module to be updated", args_param_specs_nargs | args_param_specs_require, param_type_str);

	export = args_parser_add_sub(result, "update", "the module to they latest version");
	args_add_param(setup, "dir", "the of the module to be used", 0, param_type_str);

	return (result);
}

int	_arsg_extract(
	t_args_output	*const	out,
	Config			*const	config
)
{
	t_args_output_option	*output = args_get_option(out, "output");
	t_args_output_option	*file = args_get_option(out, "config");
	int						n = 0;

	if (output)
		config->dest = args_get_param(output, "path", &n);

	if (!config->dest)
		config->dest = ".";

	if (file)
		config->config_file = args_get_param(output, "path", &n);

	if (!config->config_file)
		config->config_file = ".clib";

	if (!strcmp("setup", args_active_subcommand(output)))
		config->sub = SETUP;
	else if (!strcmp("update", args_active_subcommand(output)))
		config->sub = UPDATE;
	else if (!strcmp("export", args_active_subcommand(output)))
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
			fprintf(stderr, "Error: missing remote `url` field in the config file\n");
			return (EINVAL);
		}
	}

	toml_foreach(array, f->toml)
	{
		
	}
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
		perror("openning config file");
		return (errno);
	}

	switch (config.sub)
	{
	case SETUP:
		/* code */
		break;
	case UPDATE:
		/* code */
		break;
	case EXPORT:
		/* code */
		break;
	default:
		fprintf(stderr, "wtf did the parser just got ? Or did i never update the dispatch logic after adding a new sub parser ?\nany way here the sub command: '%s'\n", args_active_subcommand(output));
		exit_status = EINVAL;
		break;
	}

	args_parser_free(parser);
	args_output_free(output);
	return (exit_status);
}

