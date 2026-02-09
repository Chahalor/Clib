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
	if (unlikely(!_parser || _argc < 1 || !_argv))
	{
		_args_config_set_errnum(error_invalid_arg);
		return (NULL);
	}

	return (_args_parse(_parser, _argc, _argv));
}

void	args_show_help(
	const t_args_parser *const _parser,
	const int _exit
)
{
	_t_args_config	*_config = _args_config_get();

	_config->exit_on_help = _exit;
	_args_builtin_help(_parser, _config, 0);
}
