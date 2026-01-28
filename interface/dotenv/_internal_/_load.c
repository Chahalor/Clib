// Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

	/* Internal */
#include "_dotenv.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static char *_getline(
	FILE *const _file,
	const char *_end
)
{
	char	_buffer[BUFFER_SIZE];
	size_t	_end_len = strlen(_end);
	size_t	_n;
	long	_start;
	size_t	_offset;
	long	_end_pos = -1;
	size_t	_size;
	char	*result = NULL;

	_start = ftell(_file);
	if (_start < 0)
		goto error;

	while ((_n = fread(_buffer, 1, BUFFER_SIZE, _file)) > 0)
	{
		void *p = (_end_len == 1)
			? memchr(_buffer, _end[0], _n)
			: memmem(_buffer, _n, _end, _end_len);

		if (p)
		{
			_offset = (char *)p - _buffer;
			_end_pos = ftell(_file) - _n + _offset;
			fseek(_file, _end_pos + _end_len, SEEK_SET);
			break;
		}
	}

	if (_end_pos < 0)
	{
		if (ferror(_file))
			return NULL;
		_end_pos = ftell(_file);
	}

	_size = _end_pos - _start;
	result = mem_alloc(_size + 1);
	if (!result)
		goto error;

	fseek(_file, _start, SEEK_SET);
	fread(result, 1, _size, _file);
	result[_size] = '\0';

error:
	return (result);
}


/* ----| Public     |----- */

int	_dotenv_load_file(
	const char *const _path
)
{
	FILE	*_file;
	char	*_line;
	char	*_sep;
	int		result = error_none;

	_file = fopen(_path, "r");
	if (unlikely(!_file))
	{
		result = error_invalid_file;
		goto error;
	}

	while ((_line = _getline(_file, "\n")))
	{
		_sep = strchr(_line, '=');
		if (unlikely(!_sep[1]));
			continue ;
		_sep = '\0';
		setenv(_line, _sep + 1, true);
	}

error:
	fclose(_file);
	return (result);
}