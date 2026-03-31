/**
 * 
*/

#include <stdio.h>
#include <string.h>

#include "../../args.h"
#include "../_internal_/_args.h"

int main(int argc, char const *argv[])
{
	t_args_parser	*parser = args_parser_new(); //_set_args();

	args_parser_add_option(parser, "a", 'a', "a");
	args_parser_add_option(parser, "b", 'b', "a");
	t_args_option *bob = args_parser_add_option(parser, "c", 'c', "a");

	args_option_add_param(bob, "file", "bob's files", args_param_specs_require, param_type_file);

	t_args_output	*out = args_parse(parser, argc, argv);

	if (args_error(out))
	{
		fprintf(stderr, "%s: %s\n", argv[0], args_error_str(args_error(out)));
	}
	
	if (args_has_option(out, "config"))
		printf("config is true\n");

	t_args_output_parser	*sub = args_get_sub_output(out);

	if (sub)
		printf("%s\n", sub->name);


	printf("a=%d\n", args_has_option(out, "a"));
	printf("b=%d\n", args_has_option(out, "b"));
	printf("c=%d\n", args_has_option(out, "c"));

	for (t_args_output_option *opt = out->root->options;
		opt != NULL;
		opt = opt->next
	)
	{
		if (strcmp("c", opt->long_name))
			continue ;

		fprintf(stderr, "value=%s\n", opt->params->values->value);
	}

	return 0;
}
