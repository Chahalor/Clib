// Big Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>

	/* Internal */
#include "_internal_/_io.h"
#include "io.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

//TODO: manager call have to be changed to va_arg
s_file	*file_open(
	const char *const restrict _filename,
	const int _mode,
	...
)
{
	s_file	*result;
	va_list	_list;
	int		_perms = 0;

	if (unlikely(!_filename))
		return (error_invalid_arg);
	
	if (_mode & O_CREAT)
	{
		va_start(_list, _mode);
		_perms = va_arg(_list, int);
		va_end(_list);
	}
	return (file_manager(_file_storage_open, (void *)&result, _filename, _mode, _perms));
}

int	file_close(
	s_file *const restrict _file

)
{
	if (unlikely(!_file))
		return (error_invalid_arg);

	return (_file_manager(_file_storage_close, NULL, _file));
}

int	file_is_open(
	const char *const restrict _filename
)
{
	int	result = 0;

	if (unlikely(!_filename))
		return (0);

	_file_manager(_file_storage_is_open, &result, _filename);
	return (result);
}

ssize_t	file_write(
	s_file *const restrict _file,
	const void *const restrict _buffer,
	const size_t _size
)
{
	ssize_t	result = 0;
	int		_error = error_none;

	if (unlikely(!_file || !_buffer || _size < 0))
		return (error_invalid_arg);
	else if (unlikely(!_size))
		return (error_none);

	_error = _file_manager(_file_storage_write, &result, _file, _buffer, _size);
	return (_error != error_none
			? _error
			: result);
}

ssize_t	file_printf(
	s_file *const restrict _file,
	const char *const restrict _format,
	...
)
{

	va_list	_list;
	ssize_t	result = 0;

	if (unlikely(!_file || !_format))
		return (error_invalid_arg);

	va_start(_list, _format);



	va_end(_list);
	return (result);
}
