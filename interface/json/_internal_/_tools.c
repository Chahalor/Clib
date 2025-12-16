// Header

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>

	/* Internal */
#include "_json.h"
	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

int	_json_tool_count_field(
	const char *const restrict _str
)
{
	int		count = 0;
	bool	_in_segment = false;
	size_t	_i = 0;

	if (unlikely(!_str))
		return (0);

	while (_str[_i])
	{
		if (!_json_tool_is_sep(_str[_i]))
		{
			if (!_in_segment)
			{
				count++;
				_in_segment = true;
			}
		}
		else
			_in_segment = false;
		_i++;
	}
	return (count);
}


char	*_json_tool_itoa(
	long long _value
)
{
	char		*result;
	long long	_dummy = _value;
	int			_size = _dummy == 0;

	while (_dummy)
	{
		_size++;
		_dummy /= 10;
	}

	result = mem_alloc(sizeof(char) * (_size + 1));
	if (unlikely(!result))
		return (NULL);
	while (_value)
	{
		result[--_size] = (_value % 10) + '0';
		_value /= 10;
	}
	return (result);
}

char	**_json_tool_split(
	const char *const restrict _field
)
{
	char	**result;
	size_t	_len = strlen(_field);
	int		_nb_sep = 1;
	char	*_str_zone;
	size_t	_i = 0;
	size_t	_part = 0;
	size_t	_start = 0;
	size_t	_zone_offset;
	size_t	_part_len;

	_nb_sep = _json_tool_count_field(_field);

	result = mem_alloc(sizeof(char *) * (_nb_sep + 1) + (_len + _nb_sep));
	if (unlikely(!result))
		return (NULL);

	_str_zone = (char *)(result + (_nb_sep + 1));
	_zone_offset = 0;

	while (_field[_i])
	{
		_start = _i;
		while (_field[_i] && !_json_tool_is_sep(_field[_i]))
			_i++;

		_part_len = _i - _start;
		result[_part] = _str_zone + _zone_offset;
		memcpy(result[_part], &_field[_start], _part_len);
		result[_part][_part_len] = '\0';
		_zone_offset += _part_len + 1;
		_part++;
		if (likely(_field[_i]))
			_i++;
	}
	result[_part] = NULL;
	return (result);
}

size_t _json_tool_array_len(
	const void *_array,
	const int _elt_size
)
{
	size_t	i = 0;

	if (unlikely(!_array || _elt_size == 0))
		return (0);

	while (*(void * const *)((char *)_array + i * _elt_size))
		i++;

	return (i);
}

