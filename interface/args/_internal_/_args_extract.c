// Header

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>

	/* Internal */
#include "_args.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static inline unsigned int	_count_values(
	_t_args_output_value *_values
)
{
	unsigned int	result = 0;

	for (_t_args_output_value	*_this = _values;
		_this;
		_this = _this->next
	)
	{
		result++;
	}

	return (result);
}

static inline char	**_merge_params(
	_t_args_output_param *const _params,
	unsigned int *const _count
)
{
	unsigned int	_total = 0;
	unsigned int	_i = 0;
	char			**result = NULL;

	if (_count)
		*_count = 0;
	if (!_params || !_count)
		return (NULL);

	for (_t_args_output_param	*_this = _params;
		_this;
		_this = _this->next
	)
	{
		_total += _count_values(_this->values);
	}

	result = mem_alloc(sizeof(char *) * (_total + 1));
	if (unlikely(!result))
	{
		_args_config_set_errnum(error_alloc_fail);
		return (NULL);
	}

	for (_t_args_output_param *_this = _params;
		_this;
		_this = _this->next
	)
	{
		for (_t_args_output_value *_val = _this->values;
			_val;
			_val = _val->next
		)
		{
			result[_i++] = _val->value;
		}
	}
	result[_i] = NULL;
	*_count = _total;
	return (result);

}

/* ----| Public     |----- */

char	*_args_get_param(
	t_args_output *const _output,
	const char *const _name,
	char *const * *const _values,
	unsigned int *const _count
)
{
	unsigned int	_nb_values = 0;
	char			**result = NULL;
	unsigned int	_i = 0;
	char			name = NULL;

	for (_t_args_output_param	*_this = _output->params;
		_this;
		_this = _this->next
	)
	{
		if (strcmp(_this->name, _name))
			continue ;

		_nb_values = _count_values(_this->values);
		result = mem_alloc(sizeof(char *) * (_nb_values + 1));
		if (unlikely(!result))
		{
			_args_config_set_errnum(error_alloc_fail);
			goto error;
		}
		for (_t_args_output_value	*_this = _this->value;
		_this;
		_this = _this->next
		)
		{
			result[_i++] = _this->value;
		}
		result[_i] = NULL;
		*_values = result;
		*_count = _nb_values;
		name = _this->name;
		break ;
	}

error:
	return (name);
}

char	_args_get_option(
	t_args_output *const _output,
	const char *const _name,
	char *const * *const _values,
	unsigned int *const _count
)
{
	unsigned int	_nb_values;
	char			*_lname;
	char			_key;
	char			*name = NULL;

	if (_name[0] == '-')
	{
		if (_name[1] == '-')
			_lname = _name + 2;
		else
			_key = _name[1];
	}
	else if (strlen(_name) > 1)
		_lname = _name + 1;
	else
		_key = _name[0];

	for (_t_args_output_option	*_this = _output->options;
		_this;
		_this = _this->next
	)
	{
		if (likely(_lname && _this->long_name && strcmp(_this->long_name, _lname)))
			continue ;
		else if(likely(_this->short_name != _key))
			continue ;

		if (_this->params)
		{
			*_values = _merge_params(_this->params, _count);
		}
		else
		{
			*_values = (char **)0x1;
			_nb_values = 0;
		}
	}

	return (0);
}

const char	*_args_active_subcommand(
	const t_args_output *_out
)
{
	char	*result;

	if (_out->sub)
		result = _out->sub->name;
	else
		result = NULL;

	return (result);
}

t_args_output	*_args_get_sub_output(
	const t_args_output *_out,
	const char *_name
)
{
	t_args_output	*result;

	if (_out->sub)
		result = _out->sub;
	else
		result = NULL;

	return (result);
}
