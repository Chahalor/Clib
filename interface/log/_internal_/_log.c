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

/*
	TODO: use
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	strftime(buffer, sizeof(buffer),
             "[%Y-%m-%d %H:%M:%S]", t);
 */

// [year-mouth-day hour:minute:second] <ERROR> <function> (file:line)  code=<code> - <Logs summary>
#ifdef DEBUG
# define	FORMAT_SUM_TTY	GREY "%*s[%s]" RESET " - %s%s - %s - " GREY "(%s:%d)" RESET " code=%d - %s\n"
# define	FORMAT_SUM_FILE	     "%*s[%s] - "         "%s - %s - "      "(%s:%d)"       " code=%d - %s\n"
#else
# define	FORMAT_SUM_TTY	GREY "%*s[%s]" RESET " - %s%s code=%d - %s\n"
# define	FORMAT_SUM_FILE	     "%*s[%s] - "         "%s code=%d - %s\n"
#endif	// DEBUG
//								"(depth * \t)|<line x>"
# define	FORMAT_BODY_TTY		"%*s%s|%*s\n"
# define	FORMAT_BODY_FILE	"%*s|%*s\n"

static inline const char	*_log_level_str(
	const t_log_level level
)
{
	const char	*levels[5] = {
		[log_error] = "ERROR",
		[log_warning] = "WARN",
		[log_info] = "INFO",
		[log_debug] = "DEBUG",
		[log_other] = "OTHER"
	};

	return (level <= log_other ?
			levels[level] :
			levels[log_other]
		);
}

static inline const char	*_log_level_color(
	const t_log_level level
)
{
	const char	*colors[5] = {
		[log_error] = RED,
		[log_warning] = YELLOW,
		[log_info] = BLUE,
		[log_debug] = GREEN,
		[log_other] = ""
	};

	return (level <= log_other ?
			colors[level] :
			colors[log_other]
		);
}

#include <stdlib.h>
#include <string.h>

size_t	_line_len(
	const char *src
)
{
	size_t	result = 0;

	for (size_t	_i = 0;
		src[_i] != '\0' && src[_i] != '\n';
		_i++
	)
	{
		result++;
	}	

	return (result);
}

/* static int	_setup_log_file(
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
} */



/* ----| Public     |----- */

// t_log_internal	*_logs_config(
// 	const t_log_init *const config
// )
// {
// 	static t_log_internal	this;

// 	if (config)
// 	{
// 		// TODO: configure the logs files
// 	}

// 	return (&this);
// }

int	_logs_print(
	const t_log_report *report,
	const int fd,
	const int depth
)
{
	const bool	_tty = isatty(fd);
	const char	*_color = _log_level_color(report->level);
	time_t		_now = time(NULL);
	struct tm	*_t = localtime(&_now);
	char		*_format_sum;
	char		*_format_body;
	char		_buff[BUFFER_SIZE];
	size_t		_ll = 0;
	size_t		_offset = 0;

	if (_tty)
	{
		_format_sum = FORMAT_SUM_TTY;
		_format_body = FORMAT_BODY_TTY;
	}
	else
	{
		_format_sum = FORMAT_SUM_FILE;
		_format_body = FORMAT_BODY_FILE;
	}

	strftime(_buff, sizeof(_buff), "[%Y-%m-%d %H:%M:%S]", _t);

	#ifdef DEBUG	// TODO: check if tty
		dprintf(fd, _format_sum, depth, "\t", _buff, _color, _log_level_str(report->level), report->func, report->file, report->line, report->code, report->summary);
	#else
		dprintf(fd, _format_sum, depth, "\t", _buff, _color, _log_level_str(report->level), report->code, report->summary);
	#endif	// DEBUG


	while (report->body[_offset])
	{
		_ll = _line_len(report->body + _offset);

		if (_tty)
			dprintf(fd, _format_body, depth, "\t",
					_log_level_color(report->level),
					(int)_ll, report->body + _offset);
		else
			dprintf(fd, _format_body, depth, "\t",
					(int)_ll, report->body + _offset);

		_offset += _ll;

		if (report->body[_offset] == '\n')
			_offset++;
	}

	if (report->sub)
		_logs_print(report->sub, fd, depth + 1);

	return (0);
}
