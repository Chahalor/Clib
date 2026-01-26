// Header

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>
#include <stdio.h>

	/* Internal */
#include "_args.h"

	/* External */
#include "memory.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

static void	_args_mem_free_params(
	_t_args_param *const _params
)
{
	_t_args_param	*_this = _params;
	_t_args_param	*_next = NULL;

	while (_this)
	{
		_next = _this->next;
		if (_this->desc)
			mem_free(_this->desc);
		mem_free(_this);
		_this = _next;
	}
}

static void	_args_mem_free_options(
	_t_args_option *const _options
)
{
	_t_args_option	*_this = _options;
	_t_args_option	*_next = NULL;

	while (_this)
	{
		_next = _this->next;
		if (_this->params)
			_args_mem_free_params(_this->params);
		if (_this->desc)
			mem_free(_this->desc);
		mem_free(_this);
		_this = _next;
	}
}

static void	_args_mem_free_output_values(
	_t_args_output_value *const _values
)
{
	_t_args_output_value	*_this = _values;
	_t_args_output_value	*_next = NULL;

	while (_this)
	{
		_next = _this->next;
		mem_free(_this);
		_this = _next;
	}
}

static void	_args_mem_free_output_params(
	_t_args_output_param *const _params
)
{
	_t_args_output_param	*_this = _params;
	_t_args_output_param	*_next = NULL;

	while (_this)
	{
		_next = _this->next;
		if (_this->values)
			_args_mem_free_output_values(_this->values);
		mem_free(_this);
		_this = _next;
	}
}

static void	_args_mem_free_output_options(
	_t_args_output_option *const _options
)
{
	_t_args_output_option	*_this = _options;
	_t_args_output_option	*_next = NULL;

	while (_this)
	{
		_next = _this->next;
		if (_this->params)
			_args_mem_free_output_params(_this->params);
		mem_free(_this);
		_this = _next;
	}
}

static void	_args_mem_free_output_parsers(
	_t_args_output_parser *const _parsers
)
{
	_t_args_output_parser	*_this = _parsers;
	_t_args_output_parser	*_next = NULL;

	while (_this)
	{
		_next = _this->next;
		if (_this->options)
			_args_mem_free_output_options(_this->options);
		if (_this->params)
			_args_mem_free_output_params(_this->params);
		if (_this->sub)
			_args_mem_free_output_parsers(_this->sub);
		mem_free(_this);
		_this = _next;
	}
}

_t_args_parser	*_args_mem_new_parser(
	const char *const _name,
	const char *const _desc
)
{
	const size_t	_name_length = _name ? strlen(_name) : 0;
	const size_t	_name_size = _name ? _name_length + 1 : 0;
	const size_t	_desc_length = _desc ? strlen(_desc) : 0;
	const size_t	_alloc_size = sizeof(_t_args_parser) + sizeof(char) * (_name_size);
	_t_args_parser	*new;

	new = mem_alloc(_alloc_size);
	if (unlikely(!new))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	new->name = NULL;
	new->desc = NULL;
	if (_name)
	{
		new->name = (char *)(new + 1);
		strcpy(new->name, _name);
	}
	if (_desc)
	{
		new->desc = mem_alloc(_desc_length + 1);
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

	_args_parser_add_option(new, "help", 'h', "show this message");

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
	const size_t	_name_size = _long_name ? _name_length + 1 : 0;
	const size_t	_desc_length = _desc ? strlen(_desc) : 0;
	const size_t	_alloc_size = sizeof(_t_args_option) + sizeof(char) * (_name_size);
	_t_args_option	*new;

	new = mem_alloc(_alloc_size);
	if (unlikely(!new))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	new->long_name = NULL;
	new->desc = NULL;
	if (_long_name)
	{
		new->long_name = (char *)(new + 1);
		strcpy(new->long_name, _long_name);
	}
	if (_desc)
	{
		new->desc = mem_alloc(_desc_length + 1);
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
	const t_args_param_specs _args_spec,
	const t_param_type _type
)
{
	const size_t	_name_length = _name ? strlen(_name) : 0;
	const size_t	_name_size = _name ? _name_length + 1 : 0;
	const size_t	_desc_length = _desc ? strlen(_desc) : 0;
	const size_t	_alloc_size = sizeof(_t_args_param) + sizeof(char) * (_name_size);
	_t_args_param	*new;

	new = mem_alloc(_alloc_size);
	if (unlikely(!new))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	new->name = NULL;
	new->desc = NULL;
	if (_name)
	{
		new->name = (char *)(new + 1);
		strcpy(new->name, _name);
	}
	if (_desc)
	{
		new->desc = mem_alloc(_desc_length + 1);
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
	new->specs = _args_spec;
	new->type = _type;

error:
	return (new);
}

_t_args_output_value	*_args_mem_new_out_val(
	const char *const _value
)
{
	const size_t			_alloc_size = sizeof(_t_args_output_value) +
		(_value ? strlen(_value) + 1 : 0);
	_t_args_output_value	*result;

	result = mem_alloc(_alloc_size);
	if (unlikely(!result))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	result->value = NULL;
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
	const size_t			_alloc_size = sizeof(_t_args_output_param) +
		(_name ? strlen(_name) + 1 : 0);
	_t_args_output_param	*result;

	result = mem_alloc(_alloc_size);
	if (unlikely(!result))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	result->name = NULL;
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
	const size_t			_alloc_size = sizeof(_t_args_output_option) +
		(_opt && _opt->long_name ? strlen(_opt->long_name) + 1 : 0);
	_t_args_output_option	*result;

	result = mem_alloc(_alloc_size);
	if (unlikely(!result))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	result->long_name = NULL;
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
	const size_t			_alloc_size = sizeof(_t_args_output_parser) +
		(_name ? strlen(_name) + 1 : 0);
	_t_args_output_parser	*result;

	result = mem_alloc(_alloc_size);
	if (unlikely(!result))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	result->name = NULL;
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

_t_args_output	*_args_mem_new_output(void)
{
	_t_args_output	*result;

	result = mem_alloc(sizeof(_t_args_output));
	if (likely(result))
	{
		result->root = NULL;
		result->error = error_none;
	}

	return (result);
}

void	_args_mem_free_parser(
	_t_args_parser *const _parser,
	const bool _deep
)
{
	_t_args_parser	*_this = _parser;
	_t_args_parser	*_next = NULL;

	while (_this)
	{
		_next = _this->next;
		if (_deep)
		{
			if (_this->options)
				_args_mem_free_options(_this->options);
			if (_this->params)
				_args_mem_free_params(_this->params);
			if (_this->sub_parsers)
				_args_mem_free_parser(_this->sub_parsers, true);
		}
		if (_this->desc)
			mem_free(_this->desc);
		mem_free(_this);
		_this = _next;
	}
}

void	_args_mem_free_output(
	_t_args_output *const _out,
	const bool _deep
)
{
	if (!_out)
		return ;
	if (_deep && _out->root)
		_args_mem_free_output_parsers(_out->root);
	mem_free(_out);
}
