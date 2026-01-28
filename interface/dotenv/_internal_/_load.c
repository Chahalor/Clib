// Header

#define _GNU_SOURCE

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
	long	_after = -1;
	size_t	_size;
	char	*result = NULL;

	_start = ftell(_file);
	if (_start < 0)
		goto error;

	while ((_n = fread(_buffer, 1, BUFFER_SIZE, _file)) > 0)
	{
		void	*p = (_end_len == 1) ?
						memchr(_buffer, _end[0], _n) :
						memmem(_buffer, _n, _end, _end_len);

		if (p)
		{
			_offset = (char *)p - _buffer;
			_end_pos = ftell(_file) - _n + _offset;
			_after = _end_pos + _end_len;
			fseek(_file, _after, SEEK_SET);
			break;
		}
	}

	if (_end_pos < 0)
	{
		if (ferror(_file))
			return NULL;
		_end_pos = ftell(_file);
		_after = _end_pos;
		if (_end_pos == _start && _after == _start)
			return NULL;
	}

	_size = _end_pos - _start;
	result = mem_alloc(_size + 1);
	if (!result)
		goto error;

	fseek(_file, _start, SEEK_SET);
	fread(result, 1, _size, _file);
	result[_size] = '\0';
	if (_after >= 0)
		fseek(_file, _after, SEEK_SET);

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
		if (unlikely(!_sep || !_sep[0] || !_sep[1]))
		{
			mem_free(_line);
			continue ;
		}
		_sep[0] = '\0';
		setenv(_line, _sep + 1, true);
		mem_free(_line);
	}

error:
	if (_file)
		fclose(_file);
	return (result);
}
