// Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdarg.h>

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

int	args_parser_set_desc(
	t_args_parser *const _parser,
	const char *const _desc
)
{
	if (unlikely(!_parser))
		return (error_invalid_arg);
	else
		return (_args_parser_set_desc(_parser, _desc));
}

int	args_param_set_desc(
	t_args_param *const _param,
	const char *const _desc
)
{
	if (unlikely(!_param))
		return (error_invalid_arg);
	else
		return (_args_param_set_desc(_param, _desc));
}

int	args_option_set_desc(
	t_args_option *const _option,
	const char *const _desc
)
{
	if (unlikely(!_option))
		return (error_invalid_arg);
	else
		return (_args_option_set_desc(_option, _desc));
}
