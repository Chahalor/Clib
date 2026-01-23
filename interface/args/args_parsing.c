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

/** */
t_args_output	*args_parse(
	const t_args_parser *const _parser,
	const int _argc,
	const char *const _argv[]
)
{
	int	result = error_none;

	if (unlikely(!_parser))
		result = error_invalid_arg;
	else
		result = _args_parse(_parser, _argc, _argv);

	return (result);
}