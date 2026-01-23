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

static inline ARGSP	*_new_elt(
	const size_t _length
)
{
	const size_t	_alloc_size = sizeof(ARGSP) + _length;
	ARGSP			*result = NULL;

	result = mem_alloc(_alloc_size);
	if (unlikely(!result))
		_args_config_set_errnum(error_alloc_fail);

	return (result);
}

/* ----| Public     |----- */

ARGSP	*_args_mem_new_param(
	const char *const restrict _name
)
{
	const size_t	_name_len = _name ? strlen(_name) + 1 : 0;
	_t_args_param	*_this = NULL;;
	ARGSP			*result = NULL;

	result = _new_elt(sizeof(_t_args_param) + _name_len);
	if (unlikely(!result))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}
	else
	{
		result->data.param = (_t_args_param *)(result + 1);
		result->type = _e_args_data_type_param;
		_this = result->data.param;
	}

	if (_name_len)
	{
		_this->name = (char *)(_this + 1);
		strcpy(_this->name, _name);
	}
	else
		_this->name = NULL;
	_this->desc = NULL;
	_this->next = NULL;
	_this->type = 0;
	_this->values = NULL;
	_this->args_type = 0;
	_this->is_fill = false;
	_this->nb_values = 0;

error:
	return (result);
}

ARGSP	*_args_mem_new_option(
	const char *const restrict _name,
	const char _short_name
)
{
	const size_t	_name_len = _name ? strlen(_name) + 1 : 0;
	const size_t	_alloc_size = sizeof(_t_args_option) + _name_len;
	_t_args_option	*_this = NULL;
	ARGSP			*result;

	result = _new_elt(_alloc_size);
	if (unlikely(!result))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}
	else
	{
		result->data.option = (_t_args_option *)(result + 1);
		result->type = _e_args_data_type_opt;
		_this = result->data.option;
	}

	if (_name_len)
	{
		_this->long_name = (char *)(_this + 1);
		strcpy(_this->long_name, _name);
	}
	else
		_this->long_name = NULL;
	_this->desc = NULL;
	_this->params = NULL;
	_this->next = NULL;
	_this->short_name = _short_name;
	_this->nb_calls = 0;

error:
	return (result);
}

ARGSP	*_args_mem_new_parser(
	const char *const restrict _name
)
{
	const size_t	_name_len = _name ? strlen(_name) + 1 : 0;
	const size_t	_alloc_size = sizeof(_t_args_parser) + _name_len;
	_t_args_parser	*_this = NULL;
	ARGSP			*result;


	result = mem_alloc(_alloc_size);
	if (unlikely(!result))
	{
		_args_config_set_errnum(error_alloc_fail);
		goto error;
	}
	else
	{
		result->data.parser = (_t_args_parser *)(result + 1);
		result->type = _e_args_data_type_parser;
		_this = result->data.parser;
	}

	if (_name_len)
	{
		_this->name = (char *)(_this + 1);
		strcpy(_this->name, _name);
	}
	else
		_this->name = NULL;
	_this->desc = NULL;
	_this->options = NULL;
	_this->params = NULL;
	_this->sub_parsers = NULL;
	_this->next = NULL;


error:
	return (result);
}

ARGSP	*_args_mem_new_root(void)
{
	ARGSP	*result;

	result = _args_mem_new_parser(NULL);
	result->type = _e_args_data_type_root;

	return (result);
}

void	_args_mem_free_param(
	ARGSP *const restrict _param,
	const char _recursiv
)
{
	if (unlikely(!_param))
		return ;

	mem_free(_param->data.param->desc);
	if (_recursiv && _param->data.param->next)
		_args_mem_free_param(_param->data.param->next, true);
	mem_free(_param);
}

void	_args_mem_free_opt(
	ARGSP *const restrict _param,
	const char _recursiv
)
{
	if (unlikely(!_param))
		return ;

	mem_free(_param->data.option->desc);
	if (_recursiv && _param->data.option->next)
		_args_mem_free_opt(_param->data.option->next, true);
	mem_free(_param);
}

void	_args_mem_free_parser(
	ARGSP *const restrict _param,
	const char _recursiv
)
{
	if (unlikely(!_param))
		return ;

	mem_free(_param->data.parser->desc);
	if (_recursiv && _param->data.parser->next)
		_args_mem_free_parser(_param->data.parser->next, true);
	_args_mem_free_param(_param->data.parser->params, true);
	_args_mem_free_opt(_param->data.parser->options, true);
	_args_mem_free_parser(_param->data.parser->sub_parsers, true);
	mem_free(_param);
}

void	_args_remove_sub(
	ARGSP *const _main,
	ARGSP *const _sub
)
{
	ARGSP	*_prev = _main->data.parser->sub_parsers;

	for (ARGSP	*_this = _main->data.parser->sub_parsers;
		_this; 
		_this = _this->data.parser->next
	)
	{
		if (unlikely(_this == _prev))
		{
			_prev->data.parser->next = _this->data.parser->next;
			_args_mem_free_parser(_this, false);
			return ;
		}
		_prev = _this;
	}
}

void	_args_remove_opt(
	ARGSP *const _main,
	ARGSP *const _opt
)
{
	ARGSP	*_prev = _main->data.parser->options;

	for (ARGSP	*_this = _main->data.parser->options;
		_this;
		_this = _this->data.option->next
	)
	{
		if (_this == _prev)
		{
			_prev->data.option->next =  _this->data.option->next;
			_args_mem_free_opt(_this, false);
			return ;
		}
		_prev = _this;
	}
}

void	_args_remove_param_from_opt(
	ARGSP *const _main,
	ARGSP *const _param
)
{
	ARGSP	*_prev = _main->data.option->params;

	for (ARGSP	*_this = _main->data.option->params;
		_this;
		_this = _this->data.option->next
	)
	{
		if (unlikely(_this = _prev))
		{
			_prev->data.option->next = _this->data.option->next;
			_args_mem_free_param(_this, false);
			return ;
		}
		_prev = _this;
	}
}

void	_args_remove_param_from_parser(
	ARGSP *const _main,
	ARGSP *const _param
)
{
	ARGSP	*_prev = _main->data.parser->params;

	for (ARGSP	*_this = _main->data.parser->params;
		_this;
		_this = _this->data.param->next
	)
	{
		if (unlikely(_this = _prev))
		{
			_prev->data.param->next = _this->data.param->next;
			_args_mem_free_param(_this, false);
			return ;
		}
		_prev = _this;
	}
}
