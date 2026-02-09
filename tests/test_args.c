#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "interface/args.h"
#include "files/json.h"

static t_args_parser	*_set_args(void)
{
	t_args_parser	*parser = args_parser_new();
	t_args_option	*opt_config = NULL;
	t_args_option	*opt_todo = NULL;
	t_args_parser	*sub_open = NULL;
	t_args_parser	*sub_close = NULL;
	t_args_parser	*sub_sync = NULL;
	t_args_parser	*sub_pull = NULL;
	t_args_option	*sub_pull_opt_todo = NULL;
	t_args_parser	*sub_push = NULL;
	t_args_parser	*sub_cli = NULL;

	if (unlikely(!parser))
		return (NULL);

	opt_config = args_parser_add_option(parser, "config", 0, "Path to the config file");
	args_option_add_param(opt_config, "path", "path to the config file", args_param_specs_require, param_type_file);
	opt_todo = args_parser_add_option(parser, "todo", 't', "the TODO file to be edited");
	args_option_add_param(opt_todo, "file", "Path to the TODO file", args_param_specs_require, param_type_file);
	args_parser_add_option(parser, "silent", 's', "silent the app execution");

	sub_open = args_parser_add_sub(parser, "open", "(re)Open an issue");
	args_parser_add_param(sub_open, "title", "The title of the issue", args_param_specs_require, param_type_str);
	args_parser_add_param(sub_open, "description", "The description of the issue", args_param_specs_require, param_type_str);
	args_parser_add_param(sub_open, "label", "the label of the issue", 0, param_type_str);
	args_parser_add_param(sub_open, "assigees", "people assigne to the issue", 0, param_type_str);
	args_parser_add_param(sub_open, "parent", "the id of the parent issue", 0, param_type_int);

	sub_close = args_parser_add_sub(parser, "close", "close the issue based of the id");
	args_parser_add_param(sub_close, "id", "the id of the issue to be closed", args_param_specs_require, param_type_int);
	args_parser_add_param(sub_close, "comment", "a comment that can be added before closing", 0, param_type_str);

	sub_sync = args_parser_add_sub(parser, "sync", "syncronise the github in local and in remote");

	sub_pull = args_parser_add_sub(parser, "pull", "pull the remote issue from github");
	sub_pull_opt_todo = args_parser_add_option(sub_pull, "todo", 't', "the TODO file to be edited");
	args_option_add_param(sub_pull_opt_todo, "file", "Path to the TODO file", args_param_specs_require, param_type_file);

	sub_push = args_parser_add_sub(parser, "push", "push the local issue to the remote github");

	sub_cli = args_parser_add_sub(parser, "cli", "open the app cli");

	return (parser);
}


/* ----| Public     |----- */
	//...

/* ----| Main       |----- */


int main(
	const int argc,
	const char *argv[]
)
{
	t_args_parser	*parser = _set_args();
	t_args_output	*out = args_parse(parser, argc, argv);
	JSON			*_config = NULL;
	int				exit_code = EXIT_SUCCESS;

	if (args_error(out))
	{
		fprintf(stderr, "%s: %s\n", argv[0], args_error_str(args_error(out)));
		exit_code = EXIT_FAILURE;
		goto error;
	}

	if (args_has_option(out, "config"))
	{
		char			**_name = NULL;
		unsigned int	_count = 0;

		args_get_option(out, "config", (char *const **)&_name, &_count);
		_config = json_load(_name[0]);
		if (unlikely(!_config))
		{
			perror("cannot load config file");
			goto error;
		}
	}
	else
		_config = json_load("config.json");

	if (args_has_option(out, "silent"))
	{
		fclose(stdout);	// TODO: move this to the usage of the module Logs
	}


error:
	args_parser_free(parser);
	args_output_free(out);
	json_unload(_config);
	return (exit_code);
}
