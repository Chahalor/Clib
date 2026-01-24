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
	//...

/* ----| Public     |----- */

void	_args_init_root_config(
	_t_args_config *const _conf
)
{
	*_conf = (_t_args_config){
		0	// TODO: default config
	};
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

_t_args_param	*_args_mem_new_param(
	const char *const _name,
	const char *const _desc,
	const t_param_args_type _args_spec,
	const t_param_type _type
)
{
	const size_t	_name_length = _name ? strlen(_name) : 0;
	const size_t	_desc_length = _desc ? strlen(_desc) : 0;
	const size_t	_alloc_size = sizeof(_t_args_parser) + sizeof(char) * (_name_length);
	_t_args_param	*new;

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
	new->args_type = _args_spec;
	new->type = _type;

error:
	return (new);
}

_t_args_output_value	*_args_mem_new_out_val(
	const char *const _value
)
{
	const size_t			_alloc_size = sizeof(_t_args_output_value) + _value ? strlen(_value) : 0;
	_t_args_output_value	*result;

	result = mem_alloc(_alloc_size);
	if (unlikely(!result))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	if (_value)
	{
		result->value = (char *)(result + 1);
		strcpy(result->value, _value);
	}
	result->next = NULL;

error:
	return (result);
}

_t_args_output_param	*_args_mem_new_out_param(
	const char *const _name
)
{
	const size_t			_alloc_size = sizeof(_t_args_output_param) + _name ? strlen(_name) : 0;
	_t_args_output_param	*result;

	result = mem_alloc(_alloc_size);
	if (unlikely(!result))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	if (_name)
	{
		result->name = (char *)(result + 1);
		strcpy(result->name, _name);
	}
	result->values = NULL;
	result->error = error_none;
	result->next = NULL;

error:
	return (result);
}

_t_args_output_option	*_args_mem_new_out_opt(
	const _t_args_option *const _opt
)
{
	const size_t			_alloc_size = sizeof(_t_args_output_option) + _opt->long_name ? strlen(_opt->long_name) : 0;
	_t_args_output_option	*result;

	result = mem_alloc(_alloc_size);
	if (unlikely(!result))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	if (_opt->long_name)
	{
		result->long_name = (char *)(result + 1);
		strcpy(result->long_name, _opt->long_name);
	}
	result->short_name = _opt->short_name;
	result->params = NULL;
	result->error = error_none;
	result->next = NULL;

error:
	return (result);
}

_t_args_output_parser	*_args_mem_new_out_parser(
	const char *const _name
)
{
	const size_t			_alloc_size = sizeof(_t_args_output_parser) + _name ? strlen(_name) : 0;
	_t_args_output_parser	*result;

	result = mem_alloc(_alloc_size);
	if (unlikely(!result))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	if (_name)
	{
		result->name = (char *)(result + 1);
		strcpy(result->name, _name);
	}
	result->options = NULL;
	result->params = NULL;
	result->sub = NULL;
	result->next = NULL;

error:
	return (result);
}