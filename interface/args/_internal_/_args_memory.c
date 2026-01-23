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

_t_args_parser	*_args_mem_new_root(void)
{
	_t_args_parser	*result;

	result = mem_alloc(sizeof(_t_args_parser));
	if (unlikely(!result))
		_args_config_set_errnum(error_alloc_fail);

	return (result);
}

_t_args_param	*_args_mem_new_param(
	const char *const restrict _name
)
{
	const size_t	_name_len = _name ? strlen(_name) + 1 : 0;
	const size_t	_alloc_size = sizeof(_t_args_param) + _name_len;
	_t_args_param	*result;

	result = mem_alloc(_alloc_size);
	if (unlikely(!result))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	if (_name_len)
	{
		result->name = (char *)(result + 1);
		strcpy(result->name, _name);
	}
	else
		result->name = NULL;
	result->desc = NULL;
	result->next = NULL;
	result->type = 0;
	result->values = NULL;
	result->args_type = 0;
	result->is_fill = false;
	result->nb_values = 0;

error:
	return (result);
}

_t_args_option	*_args_mem_new_option(
	const char *const restrict _name,
	const char _short_name
)
{
	const size_t	_name_len = _name ? strlen(_name) + 1 : 0;
	const size_t	_alloc_size = sizeof(_t_args_option) + _name_len;
	_t_args_option	*result;

	result = mem_alloc(_alloc_size);
	if (unlikely(!result))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	if (_name_len)
	{
		result->long_name = (char *)(result + 1);
		strcpy(result->long_name, _name);
	}
	else
		result->long_name = NULL;
	result->desc = NULL;
	result->params = NULL;
	result->next = NULL;
	result->short_name = _short_name;
	result->nb_calls = 0;

error:
	return (result);
}

_t_args_parser	*_args_mem_new_parser(
	const char *const restrict _name
)
{
	const size_t	_name_len = _name ? strlen(_name) + 1 : 0;
	const size_t	_alloc_size = sizeof(_t_args_parser) + _name_len;
	_t_args_parser	*result;


	result = mem_alloc(_alloc_size);
	if (unlikely(!result))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}

	if (_name_len)
	{
		result->name = (char *)(result + 1);
		strcpy(result->name, _name);
	}
	else
		result->name = NULL;
	result->desc = NULL;
	result->options = NULL;
	result->params = NULL;
	result->sub_parsers = NULL;
	result->next = NULL;


error:
	return (result);
}

void	_args_mem_free_param(
	_t_args_param *const restrict _param,
	const char _recursiv
)
{
	if (unlikely(!_param))
		return ;
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
	if (unlikely(!_param))
		return ;
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
	if (unlikely(!_param))
		return ;
	mem_free(_param->desc);
	if (_recursiv && _param->next)
		_args_mem_free_parser(_param->next, true);
	_args_mem_free_param(_param->params, true);
	_args_mem_free_opt(_param->options, true);
	_args_mem_free_parser(_param->sub_parsers, true);
	mem_free(_param);
}

void	_args_remove_sub(
	_t_args_parser *const _main,
	_t_args_parser *const _sub
)
{
	_t_args_parser	*_prev = _main->sub_parsers;

	for (_t_args_parser	*_this = _main->sub_parsers;
		_this; 
		_this = _this->next
	)
	{
		if (unlikely(_this == _prev))
		{
			_prev->next = _this->next;
			_args_mem_free_parser(_this, false);
			return ;
		}
		_prev = _this;
	}
}

void	_args_remove_opt(
	_t_args_parser *const _main,
	_t_args_option *const _opt
)
{
	_t_args_option	*_prev = _main->options;

	for (_t_args_option	*_this = _main->options;
		_this;
		_this = _this->next
	)
	{
		if (_this == _prev)
		{
			_prev->next = _this->next;
			_args_mem_free_opt(_this, false);
			return ;
		}
		_prev = _this;
	}
}

void	_args_remove_param_from_opt(
	_t_args_option *const _main,
	_t_args_param *const _param
)
{
	_t_args_param	*_prev = _main->params;

	for (_t_args_param	*_this = _main->params;
		_this;
		_this = _this->next
	)
	{
		if (unlikely(_this = _prev))
		{
			_prev->next = _this->next;
			_args_mem_free_param(_this, false);
			return ;
		}
		_prev = _this;
	}
}

void	_args_remove_param_from_parser(
	_t_args_parser *const _main,
	_t_args_param *const _param
)
{
	_t_args_param	*_prev = _main->params;

	for (_t_args_param	*_this = _main->params;
		_this;
		_this = _this->next
	)
	{
		if (unlikely(_this = _prev))
		{
			_prev->next = _this->next;
			_args_mem_free_param(_this, false);
			return ;
		}
		_prev = _this;
	}
}
