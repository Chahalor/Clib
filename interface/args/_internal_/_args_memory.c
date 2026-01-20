// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "_args.h"

	/* External */
#include "memory.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

_t_args_param	*_args_mem_new_param(
	const char *const restrict _name
)
{
	const size_t	_alloc_size = sizeof(_t_args_param)
									+ _name ? strlen(_name) : 0;
	_t_args_param	*result;

	result = mem_alloc(_alloc_size);
	if (unlikely(!result))
	{
		_args_get_config()->errnum = error_alloc_fail;
		goto error;
	}

	result->name = (char *)(result + 1);
	result->desc = NULL;
	result->next = NULL;

error:
	return (result);
}

_t_args_option	*_args_mem_new_option(
	const char *const restrict _name
)
{
	const size_t	_alloc_size = sizeof(_t_args_option)
									+ _name ? strlen(_name) : 0;
	_t_args_option	*result;

	result = mem_alloc(_alloc_size);
	if (unlikely(!result))
	{
		_args_get_config()->errnum = error_alloc_fail;
		goto error;
	}

	result->long_name = (char *)(result + 1);
	result->desc = NULL;
	result->next = NULL;

error:
	return (result);
}

_t_args_parser	*_args_mem_new_parser(
	const char *const restrict _name
)
{
	const size_t	_alloc_size = sizeof(_t_args_parser)
									+ _name ? strlen(_name) : 0;
	_t_args_parser	*result;


	result = mem_alloc(_alloc_size);
	if (unlikely(!result))
	{
		_args_get_config()->errnum = error_alloc_fail;
		goto error;
	}

	result->name = (char *)(result + 1);
	result->desc = NULL;
	result->options = NULL;
	result->params = NULL;
	result->sub_parsers = NULL;

error:
	return (result);
}

void	_args_mem_free_param(
	_t_args_param *const restrict _param,
	const char _recursiv
)
{
	mem_free(_param->desc);
	if (_recursiv && _param->next)
		_args_mem_free_param(_param->next, true);
	mem_free(_param);
}

void	_args_mem_free_opt(
	_t_args_option *const restrict _param,
	const char _recursiv
)
{
	mem_free(_param->desc);
	if (_recursiv && _param->next)
		_args_mem_free_opt(_param->next, true);
	mem_free(_param);
}

void	_args_mem_free_parser(
	_t_args_parser *const restrict _param,
	const char _recursiv
)
{
	mem_free(_param->desc);
	if (_recursiv && _param->next);
		_args_mem_free_parser(_param->next, true);
	_args_mem_free_param(_param->params, true);
	_args_mem_free_opt(_param->options, true);
	_args_mem_free_parser(_param->sub_parsers, true);
	mem_free(_param);
}
