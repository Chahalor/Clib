// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "../args.h"
#include "_internal_/_args.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */


ARGS_PARSER		*args_parser_new(void)
{
	return (_args_mem_new_parser(NULL));
}

void			args_destroy_parser(
	ARGS_PARSER *const restrict parser
)
{
	_args_mem_free_parser(parser, true);
}
