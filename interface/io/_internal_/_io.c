// Header

/* ----| Headers    |----- */
	/* Standard */
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
	/* Internal */
#include "_io.h"
	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

int	_open(
	t_file_storage *const storage,
	const char *const restrict _filename,
	const int _mode,
	const int _perms,
	s_file **_output
)
{
	s_file		*result = NULL;
	int			_fd = -1;

	_fd = open(_filename, _mode, _perms);
	if (unlikely(_fd < 0))
		return (error_invalid_file);

	result = mem_alloc(sizeof(s_file) + sizeof(char) * strlen(_filename));
	if (unlikely(!result))
		return (close(_fd), error_alloc_fail);

	result->filename = (char *)(result + 1);
	memcpy(result->filename, _filename, strlen(_filename));
	result->fd = _fd;
	result->mode = _mode;
	*_output = result;
	storage->storage[_fd] = result;
	storage->opened_files++;
	return (error_none);
}

int	_close(
	t_file_storage *const storage,
	void *_input
)
{
	s_file	*_file = (s_file *)_input;



	if (likely(_file->fd > -1))
		close(_file->fd);
	
	storage->storage[_file->fd] = NULL;
	storage->opened_files--;
	mem_free(_file);
	return (error_none);
}

int	_close_all(
	t_file_storage *const storage
)
{
	register int	_i = 0;
	int				out = error_none;

	while (_i < MAX_FD)
	{
		if (likely(storage->storage[_i]))
			out = _close(storage, storage->storage[_i]);
		_i++;
	}
	return (out);
}

int	_is_open(
	t_file_storage *const storage,
	void *_input,
	void *_output
)
{
	register int	i = 0;

	while (i < MAX_FD)
	{
		if (unlikely(storage->storage[i]))
		{
			if (unlikely(memcmp(_input,
						storage->storage[i]->filename,
						min(strlen(_input), strlen(storage->storage[i]->filename))
					)))
			{
				*(int* )(_output) = 1;
				return (error_none);
			}
		}
	}
	*(int *)(_output) = 0;
	return (error_none);
}

int	_write(
	s_file *_file,
	void *_buffer,
	size_t _size,
	void *_output
)
{
	if (_file->fd < 0)
		return (error_invalid_file);
	*(ssize_t *)(_output) = write(_file->fd, _buffer, _size);
	if (*(ssize_t *)(_output) < 0)
		return (error_invalid_file);
	else
		return (error_none);
}

int	_empty_file_buffer(
	s_file *_file,
	ssize_t *_output,
	const int _target
)
{
	if (_target == _file_storage_empty_buffer_read)
	{
		*_output = write(_file->fd, _file->rbuffer, _file->rbuff_pos);
		if (unlikely(*_output < 0))
			return (error_invalid_file);
		memset(_file->rbuffer, 0, _file->rbuff_pos);
		_file->rbuff_pos = 0;
		return (error_none);
	}
	else if (_target == _file_storage_empty_buffer_write)
	{
		*_output = write(_file->fd, _file->wbuffer, _file->wbuff_pos);
		if (unlikely(*_output < 0))
			return (error_invalid_file);
		memset(_file->wbuffer, 0, _file->wbuff_pos);
		_file->wbuff_pos = 0;
		return (error_none);
	}
	else
		return (error_invalid_arg);
}

/* ----| Public     |----- */

int	_file_manager(
	const int _access,
	void *_output,
	...
)
{
	static struct s_file_storage	storage = {0};
	va_list							_list;
	int								result = error_none;

	va_start(_list, _output);
	switch (_access)
	{
		case (_file_storage_open):
			char	*_filename;
			int		_mode;
			int		_perms;

			_filename = va_arg(_list, char *);
			_mode = va_arg(_list, int);
			if (_mode & O_CREAT)
				_perms = va_arg(_list, int);
			result = (_open(&storage, _filename, _mode, _perms, _output));
			break;

		case (_file_storage_close):
			s_file	*_file;

			_file = va_arg(_list, s_file *);
			result = (_close(&storage, _file));
			break;

		case (_file_storage_close_all):
			result = (_close_all(&storage));
			break;

		case (_file_storage_is_open):
			char	*_filename;

			_filename = va_arg(_list, char *);
			result = (_is_open(&storage, _filename, _output));
			break;

		case (_file_storage_write):
			s_file	*_file;
			void	*_buffer;
			size_t	_size;

			_file = va_arg(_list, char *);
			_buffer = va_arg(_list, void *);
			_size = va_arg(_list, size_t);
			result = (_write(_file, _buffer, _size, _output));
			break;

		case (_file_storage_empty_buffer_read):
			s_file	*_file;

			_file = va_arg(_list, s_file *);
			result = _empty_file_buffer(_file, (ssize_t *)_output, _file_storage_empty_buffer_read);
			break;

		case (_file_storage_empty_buffer_write):
			s_file	*_file;

			_file = va_arg(_list, s_file *);
			result = _empty_file_buffer(_file, (ssize_t *)_output, _file_storage_empty_buffer_write);
			break;

		default:
			result = (error_invalid_arg);
	}
	va_end(_list);
	return (result);
}
