// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "_internal_/_args.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

int	args_parse(
	ARGSP *const parser,
	const int argc,
	const char *argv[]
)
{
	int	result;

	if (unlikely(!parser || !argv || argc < 1))
		result = error_invalid_arg;
	else
		result = _args_parse(parser, argc, argv);

	return (result);
}
