// Header

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>

	/* Internal */
#include "_args.h"

	/* External */
#include "memory.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static inline void	_init_root_config(
	_t_args_config *const _conf
)
{
	*_conf = (_t_args_config){
		0	// TODO: default config
	};
}

/* ----| Public     |----- */

_t_args_root	*_args_mem_new_root(void)
{
	_t_args_root	*result;

	result = mem_alloc(sizeof(_t_args_root));
	if (unlikely(!result))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	_init_root_config(&result->config);
	result->parser = NULL;

error:
	return (result);
}

_t_args_parser	*_args_mem_new_parser(
	const char *const _name,
	const char *const _desc
)
{
	const size_t	_name_length = _name ? strlen(_name) : 0;
	const size_t	_desc_length = _desc ? strlen(_desc) : 0;
	const size_t	_alloc_size = sizeof(_t_args_parser) + sizeof(char) * (_name_length);
	_t_args_parser	*new;

	new = mem_alloc(_alloc_size);
	if (unlikely(!new))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	if (_name)
	{
		new->name = (char *)(new + 1);
		strcpy(new->name, _name);
	}
	if (_desc)
	{
		new->desc = mem_alloc(_desc_length);
		if (unlikely(!new->desc))
		{
			_args_config_set_errnum(error_alloc_fail);
			mem_free(new);
			goto error;
		}
		else
			strcpy(new->desc, _desc);
	}

	new->next = NULL;
	new->options = NULL;
	new->params = NULL;
	new->sub_parsers = NULL;

error:
	return (new);
}

_t_args_option	*_args_mem_new_option(
	const char *const _long_name,
	const char _short_name,
	const char *const _desc
)
{
	const size_t	_name_length = _long_name ? strlen(_long_name) : 0;
	const size_t	_desc_length = _desc ? strlen(_desc) : 0;
	const size_t	_alloc_size = sizeof(_t_args_parser) + sizeof(char) * (_name_length);
	_t_args_option	*new;

	new = mem_alloc(_alloc_size);
	if (unlikely(!new))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	if (_long_name)
	{
		new->long_name = (char *)(new + 1);
		strcpy(new->long_name, _long_name);
	}
	if (_desc)
	{
		new->desc = mem_alloc(_desc_length);
		if (unlikely(!new->desc))
		{
			_args_config_set_errnum(error_alloc_fail);
			mem_free(new);
			goto error;
		}
		else
			strcpy(new->desc, _desc);
	}

	new->next = NULL;
	new->params = NULL;
	new->short_name = _short_name;

error:
	return (new);
}