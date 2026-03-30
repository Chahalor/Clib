/**
 * 
*/
#include <stdio.h>

#include "../../args.h"

int main(int argc, char const *argv[])
{
	t_args_parser	*parser = args_parser_new();

	args_parser_add_option(parser, NULL, 'a', "a");
	args_parser_add_option(parser, NULL, 'b', "a");
	args_parser_add_option(parser, NULL, 'c', "a");

	t_args_output *out = args_parse(parser, argc, argv);

	t_args_output_option	*opt;
	printf("a=%d\n", args_has_option(out, "a"));
	printf("b=%d\n", args_has_option(out, "b"));
	printf("c=%d\n", args_has_option(out, "c"));

	return 0;
}
