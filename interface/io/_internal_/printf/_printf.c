// Big Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdarg.h>
#include <string.h>
	/* Internal */
#include "_printf.h"
	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

int	_append(
	s_file *_file,
	const char *_s,
	const int _mode,
	int *result
)
{
	register int	_i = 0;
	int				_dummy;

	if (_mode == e_file_printf_append_char)
		_file->wbuffer[_file->wbuff_pos++] = *_s;
	else
	{
		while (_s[_i])
		{
			if (_file->wbuff_pos == FILE_BUFFER_WRITE_SIZE)
			{
				_file_manager(
					_file_storage_empty_buffer_write,
					&_dummy,
					_file
				);
				if (_dummy != error_none)
					return (_dummy);
				*result += _dummy;
			}
			_file->wbuffer[_file->wbuff_pos++] = _s[_i];
			_i++;
		}
	}
	if (_file->wbuff_pos == FILE_BUFFER_WRITE_SIZE)
	{
		_file_manager(
			_file_storage_empty_buffer_write,
			&_dummy,
			_file
		);
		if (_dummy != error_none)
			return (_dummy);
		*result += _dummy;
	}
	return (error_none);
}

int	_dispatch(
	s_file *const restrict _file,
	const char *const restrict _format,
	va_list _list
)
{
	size_t	_i = 0;
	char	_current;

	while (_format[_i])
	{
		_current = _format[_i];
		switch (_current)
		{
			case ('s'):
				break;
			default:
				break;
		}
	}
}

int	_printf(
	s_file *const restrict _file,
	const char *const restrict _format,
	va_list _list
)
{
	register int	_i = 0;

	while (_format[_i])
	{
		_i++;
	}
	
}
