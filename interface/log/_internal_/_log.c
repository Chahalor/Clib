// Header

/* ----| Headers    |----- */
	/* Standard */
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
	/* Internal */
#include "_log.h"
	/* External */
#include "formating.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static int	_setup_log_file(
	const t_log_file *const restrict _file,
	int	*const restrict _fd
)
{
	if (_file->filename)
		(*_fd) = open(_file->filename, O_CREAT | O_APPEND | O_RDWR, 0644);
	else
		*_fd = STDERR_FILENO;
	return (error_none);
}

static inline int	_init(
	t_log_internal *const config,
	t_log_init *data
)
{
	if (unlikely(!data || !config))
		return (error_invalid_arg);

	_setup_log_file(&data->error, &config->fd[log_error]);
	_setup_log_file(&data->warning, &config->fd[log_warning]);
	_setup_log_file(&data->info, &config->fd[log_info]);
	_setup_log_file(&data->debug, &config->fd[log_debug]);
	_setup_log_file(&data->other, &config->fd[log_other]);
	config->is_init = 1;

	return (error_none);
}

static void	_writer_print(
	const uint _fd,
	const uint _time,
	const int _depth,
	const int _level,
	const char *const restrict _format,
	va_list _list,
	t_log_internal *const _config
)
{
	va_list	_args;
	char	_buffer[BUFFER_SIZE];
	int		_offset = 0;

	va_copy(_args, _list);
	if (isatty(_fd))
		_offset += snprintf(_buffer, sizeof(_buffer), " ["BLUE"%d"RESET"] ", _time);
	else
		_offset += snprintf(_buffer, sizeof(_buffer), " [%d] ", _time);

	if (_depth == 1)
		_offset += snprintf(_buffer + _offset, sizeof(_buffer) - _offset, "\\_");
	else if (_depth > 1)
		_offset += snprintf(_buffer + _offset, sizeof(_buffer) - _offset, "%*s\\_", _depth, "  ");

	vsnprintf(_buffer + _offset, sizeof(_buffer) - _offset, _format, _args);
	dprintf(_fd, "%s\n", _buffer);

	if (_level <= _config->display_limit && !isatty(_fd))
	{
		_offset = 0;

		_offset += snprintf(_buffer, sizeof(_buffer), " ["BLUE"%d"RESET"] ", _time);
		if (_depth == 1)
			_offset += snprintf(_buffer + _offset, sizeof(_buffer) - _offset, "\\_");
		else if (_depth > 1)
			_offset += snprintf(_buffer + _offset, sizeof(_buffer) - _offset, "%*s\\_", _depth, "  ");
		vsnprintf(_buffer + _offset, sizeof(_buffer) - _offset, _format, _args);
		dprintf(STDERR_FILENO, "%s\n", _buffer);
	}
	va_end(_args);
}

static int	_writer(
	t_log_internal *_config,
	int _level,
	int _depth,
	const char *const restrict _format,
	va_list _list
)
{
	va_list			_args;
	int				_fd = 1;
	struct timeval	_s_time;
	uint			_time = 0;

	if (unlikely(!_config->is_init))
		return (-1);
	else if (_level < 0)
		return (log_error_invalid_logfile);
	else if (_level > LOG_NB_LEVEL)
		_level = log_debug;

	gettimeofday(&_s_time, NULL);
	_time = (_s_time.tv_sec * 1000) + (_s_time.tv_usec / 1000);
	_fd = _config->fd[_level];
	if (unlikely(_fd < 0))
		return (log_error_write_impossible);
	if (_depth < 0)
		_depth = _config->last_depth[_level] + 1;

	va_copy(_args, _list);
	_writer_print(_fd, _time, _depth, _level, (void *)_format, _args, _config);
	va_end(_args);

	_config->logs[_level]++;
	_config->last_depth[_level] = _depth;
	return (log_error_none);
}

static int _perror(
	t_log_internal *_config,
	int _level,
	int _depth,
	const char *const restrict _format,
	const int _errnum,
	va_list _list
)
{
	char		_errbuf[BUFFER_SIZE];
	const char	*_errmsg = strerror(_errnum);
	va_list		_args;
	char		*_final;
	uint		_need;
	int			result;

	_need = strlen(_errmsg) + 2 + strlen(_format) + 1;

	if (unlikely(!_config->is_init))
		return (-1);
	else if (_need < sizeof(_errbuf))
		_final = _errbuf;
	else
	{
		_final = mem_alloc(_need);
		if (unlikely(!_final))
			return error_alloc_fail;
	}

	snprintf(_final, _need, "%s: %s", _errmsg, _format);

	va_copy(_args, _list);
	result = _writer(_config, _level, _depth, _final, _args);
	va_end(_args);

	if (_final != _errbuf)
		mem_free(_final);

	return result;
}


int	_closer(
	t_log_internal *config,
	const int _level,
	const int _access
)
{
	if (_access == 1)
	{
		register int	i = 0;

		while (i < LOG_NB_LEVEL)
		{
			if (config->fd[i] > 2)
				close(config->fd[i]);
			i++;
		}
	}
	else
	{
		if (_level > LOG_NB_LEVEL)
			return (error_invalid_arg);
		close(config->fd[_level]);
		config->fd[_level] = -1;
	}
	return (error_none);
}


/* ----| Public     |----- */

/** */
int	_log_manager(
	const int _access,
	va_list args,
	...
)
{
	static t_log_internal	config = {0};
	va_list					_list;
	va_list					_arg;
	int						_level;
	int						_depth;
	char					*_format;
	int						result;

	va_start(_list, args);
	switch (_access)
	{
		case e_log_mng_write:
		{
			_level = va_arg(_list, int);
			_depth = va_arg(_list, int);
			_format = va_arg(_list, char *);
			va_copy(_arg, args);

			result = (_writer(&config, _level, _depth, _format, _arg));

			va_end(_arg);
			break ;
		}
		case e_log_mng_close:
		{
			_level = va_arg(_list, int);
			result = (_closer(&config, _level, _access));
			break ;
		}
		case e_log_mng_close_all:
		{
			_level = va_arg(_list, int);
			result = (_closer(&config, _level, 1));
			break ;
		}
		case e_log_mng_perror:
		{
			_level = va_arg(_list, int);
			_depth = va_arg(_list, int);
			_format = va_arg(_list, char *);
			va_copy(_arg, args);

			result = (_perror(&config, _level, _depth, _format, errno, _arg));

			va_end(_arg);
			break ;
		}

		case e_log_mng_error:
		{
			result = error_not_implemented;
			break ;
		}

		case e_log_mng_init:
		{
			result = (_init(&config, va_arg(_list, t_log_init *)));
			break ;
		}
		default:
			result = (error_invalid_arg);
	}
	va_end(_list);
	return (result);
}
