// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "_csv.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

int	_csv_alloc_root(
	CSV **_target
)
{
	CSV	*_new;

	_new = mem_alloc(sizeof(CSV));

	*_target = _new;

	return (_new ?
				error_none :
				error_alloc_fail
			);
}

int	_csv_alloc_column(
	t_csv_columns_ **_target,
	const char *const restrict _name,
	const size_t _capacity
)
{
	const size_t	_name_len = strlen(_name);
	const size_t	_total	= sizeof(t_csv_columns_)
							+ (sizeof(char) * (_name_len + 1))
							+ (sizeof(t_lib_csv_values_) * _capacity);
	t_csv_columns_	*_new;
	int				result;

	_new = mem_alloc(_total);
	if (unlikely(!_total))
	{
		result = ERROR(error_alloc_fail);
		goto error;
	}

	_new->name = (char *)(_new + 1);
	*_new = (t_csv_columns_){
		.type = CSV_T_STRING,
		.length = 0,
		.capacity = _capacity
	};
	strcpy(_new->name, _name);

error:
	return (result);
}

int	_csv_free_root(
	CSV **_target
)
{
	mem_free(_target);
	return (error_none);
}

int	_csv_free_column(
	t_csv_columns_ *const restrict _target
)
{
	mem_free(_target);
	return (error_none);
}

int	_csv_free_all(
	CSV **_target
)
{
	int	_i;
	int	_dummy;
	int	result;

	_i = 0;
	while (_i < (*_target)->nb_column)
	{
		_dummy = _csv_free_column(&(*_target)->columns[_i]);
		result = _dummy ?
					_dummy :
					result;
		_i++;
	}
	_dummy = _csv_free_root(_target);

	return (_dummy ?
				_dummy :
				result
			);
}