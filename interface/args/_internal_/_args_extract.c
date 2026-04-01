/**
 * @file _args_extract.c
 * @brief Internal implementation for the interface/args module.
 */

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
#pragma region Internal

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

void	*_get_param(
	const _t_args_output_param *const params,
	const char *const name,
	size_t *const _count
)
{
	void	*result = NULL;

	*_count = 0;

	for (const _t_args_output_param	*_this = params;
		_this;
		_this = _this->next
	)
	{
		unsigned int	_nb_values = 0;

		if (strcmp(_this->name, name))
			continue ;

		_nb_values = _count_values(_this->values);

		if (!_nb_values)
		{
			*_count = 0;
			return (NULL);
		}
		else if (_nb_values == 1)
		{
			const char	*const	_val = _this->values->value;

			result = mem_alloc(sizeof(char) * (strlen(_val) + 1));
			if (unlikely(!result))
			{
				_args_config_set_errnum(error_alloc_fail);
				goto error;
			}
			strcpy(result, _val);
			*_count = 1;

			return (result);
		}
		else
		{
			unsigned int	_i = 0;
			char			**_array = mem_alloc(sizeof(char *) * (_nb_values + 1));

			if (unlikely(!_array))
			{
				_args_config_set_errnum(error_alloc_fail);
				goto error;
			}

			for (_t_args_output_value	*_val = _this->values;
				_val;
				_val = _val->next
			)
			{
				_array[_i++] = _val->value;
			}
			_array[_i] = NULL;

				result = _array;
				*_count = _nb_values;

			return (result);
		}
	}

error:
	return (NULL);
}

t_args_output_option	*_get_options(
	const t_args_output_option *const options,
	const char *const name
)
{
	char	*_lname = NULL;
	char	_key = 0;

	if (name[0] == '-')
	{
		if (name[1] == '-')
			_lname = (char *)(name + 2);
		else
			_key = name[1];
	}
	else if (strlen(name) > 1)
		_lname = (char *)name;
	else
		_key = name[0];

	for (_t_args_output_option	*_this = (_t_args_output_option *)options;
		_this;
		_this = _this->next
	)
	{
		if (_lname && _this->long_name && strcmp(_this->long_name, _lname))
			continue ;
		else if (_key && _this->short_name != _key)
			continue ;

		return (_this);
	}

	return (NULL);
}

/* ----| Public     |----- */
#pragma region Public

void	*_args_get_param(
	t_args_output *const _output,
	const char *const _name,
	size_t *const _count
)
{
	return (_get_param(_output->root->params, _name, _count));
}

void	*_args_output_parser_get_param(
	t_args_output_parser *const _output,
	const char *const _name,
	size_t *const _count
)
{
	return (_get_param(_output->params, _name, _count));
}

void	*_args_output_option_get_param(
	t_args_output_option *const option,
	const char *const name,
	size_t *const count
)
{
	return (_get_param(option->params, name, count));
}

t_args_output_option	*_args_get_option(
	t_args_output *const _output,
	const char *const _name
)
{
	return (_get_options(_output->root->options, _name));
}

t_args_output_option	*_args_output_parser_get_option(
	t_args_output_parser *const _output,
	const char *const _name
)
{
	return (_get_options(_output->options, _name));
}

const char	*_args_active_subcommand(
	const t_args_output *_out
)
{
	const char	*result;

	if (_out && _out->root && _out->root->sub)
		result = _out->root->sub->name;
	else
		result = NULL;

	return (result);
}

_t_args_output_parser	*_args_get_sub_output(
	const t_args_output *_out
)
{
	_t_args_output_parser	*result;

	if (_out && _out->root)
		result = _out->root->sub;
	else
		result = NULL;

	return (result);
}

t_args_option	*_args_parser_get_option(
	const t_args_parser *const parser,
	const char *const name
)
{
	const char	*const	_lname = name[0] == '-' && name[1] == '-'	? name + 2	: name;
	const char			_sname = name[0] == '-' && name[1]			? name[1]	: name[0];

	for (t_args_option	*_opt = parser->options;
		_opt != NULL;
		_opt = _opt->next
	)
	{
		
		if (_opt->long_name && !strcmp(_opt->long_name, _lname))
			return (_opt);

		if (_opt->short_name == _sname)
			return (_opt);
	}

	return (NULL);
}

t_args_param	*_args_parser_get_param(
	const t_args_parser *const parser,
	const char *const name
)
{
	for (t_args_param	*_param = parser->params;
		_param != NULL;
		_param = _param->next
	)
	{
		
		if (_param->name && !strcmp(_param->name, name))
			return (_param);
	}

	return (NULL);
}
