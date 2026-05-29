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

	switch (config.sub.name)
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
	{
		err = config_load(&config, config.consts.path_config_file, false);
		if (unlikely(err))
			return (err);

		err = modules_load(&config, config.consts.path_modules_file);
		if (unlikely(err))
			return (err);

		const char	*s = config.lib.modules.length > 1 ? "s" : "";
		printf("Clib: Module%s available%s: %u\n", s, s, config.lib.modules.length);
		for (size_t i = 0; i < config.lib.modules.length; i++)
		{
			const t_module	 *const	this = config.lib.modules.data[i];

			printf(" - %s\n", this->name);
		}
		break;
	}
	default:
		args_show_help(parser, EINVAL);
		break;
	}

	args_parser_free(parser);
	args_output_free(output);
	return (exit_status);
}
