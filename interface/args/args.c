// Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

	/* Internal */
#include "../args.h"
#include "_internal_/_args.h"

	/* External */
#include "memory.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

/** */
t_args_parser	*args_new_parser(void)
{
	return (_args_mem_new_parser(NULL));
}

int	args_add_param(
	void *const restrict _parser,
	const char *const restrict _name,
	const t_param_type _type,
	const t_param_args_type _spec
)
{
	int	result;

	if (unlikely(!_parser || !_name))
	{
		result = error_invalid_arg;
		_args_config_set_errnum(result);
		goto error;
	}

	result = _args_add_param(_parser, _name, _type, _spec);

error:
	return (result);
}

int	args_add_option(
	t_args_parser *const restrict _parser,
	const char *const restrict _name,
	const t_param_type _type,
	const t_param_args_type _args_spec
)
{
	int	result;

	if (unlikely(!_parser || !_name))
	{
		result = error_invalid_arg;
		_args_config_set_errnum(result);
		goto error;
	}

	result = _args_add_param(_parser, _name, _type, _args_spec);

error:
	return (result);
}

int	args_add_subparser(
	t_args_parser *const restrict _parser,
	const char *const restrict _name
)
{
	int	result;

	if (unlikely(!_parser || !_name))
	{
		result = error_invalid_arg;
		_args_config_set_errnum(result);
		goto error;
	}

	result = _args_add_param(_parser, _name);

error:
	return (result);
}

int	args_param_add_desc(
	t_args_param **const restrict _param,
	const char *const restrict _format,
	...
)
{
	
}

int	args_option_add_desc(
	t_args_option **const restrict _opt,
	const char *const restrict _format,
	...
)
{
	
}

int	args_parser_add_desc(
	t_args_parser **const restrict _parser,
	const char *const restrict _format,
	...
)
{
	
}

int	args_parse(
	const t_args_parser *const restrict _parser,
	t_args_output *const restrict _output,
	const int argc,
	const char *argv[]
)
{
	
}

void	args_destroy_parser(
	t_args_parser *const restrict _parser
)
{
	
}
