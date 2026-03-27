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
# define	FORMAT_SUM_TTY	GREY "%s" RESET " - %s%s" RESET " - %s - " GREY "(%s:%d)" RESET " code=%d - %s\n"
# define	FORMAT_SUM_FILE	     "%s - "         "%s"       " - %s - "      "(%s:%d)"       " code=%d - %s\n"
#else
# define	FORMAT_SUM_TTY	GREY "%s" RESET " - %s%s" RESET" code=%d - %s\n"
# define	FORMAT_SUM_FILE	     "%s - "         "%s"      " code=%d - %s\n"
#endif	// DEBUG
//								"(depth * \t)|<line x>"
# define FORMAT_BODY_TTY  " %*s%s|" RESET " %.*s\n"
# define FORMAT_BODY_FILE " %*s|"         " %.*s\n"

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

/* ----| Public     |----- */

int	_logs_print(
	const t_log_report *report,
	const int fd,
	const int depth
)
{
	if (!report)
		return (0);

	const bool	_tty = isatty(fd);
	const char	*_color = _log_level_color(report->level);
	const char	*_func = report->func ? report->func : "(unknown)";
	const char	*_file = report->file ? report->file : "(unknown)";
	const char	*_summary = report->summary ? report->summary : "";
	const char	*_body = report->body ? report->body : "";
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

	if (_t)
		strftime(_buff, sizeof(_buff), "[%Y-%m-%d %H:%M:%S]", _t);
	else
		_buff[0] = '\0';

	if (depth)
		dprintf(fd, "%*s\\_", (depth * 4) - 2, "");

	#ifdef DEBUG	// TODO: check if tty
	if (_tty)
		dprintf(fd, _format_sum, _buff, _color, _log_level_str(report->level), _func, _file, report->line, report->code, _summary);
	else
		dprintf(fd, _format_sum, _buff, _log_level_str(report->level), _func, _file, report->line, report->code, _summary);
	#else
	if (_tty)
		dprintf(fd, _format_sum, _buff, _color, _log_level_str(report->level), report->code, _summary);
	else
		dprintf(fd, _format_sum, _buff, _log_level_str(report->level), report->code, _summary);
	#endif	// DEBUG

	while (_body[_offset])
	{
		const int	_indent = depth * 4;
		_ll = _line_len(_body + _offset);

		if (_tty)
			dprintf(fd, _format_body,
					_indent, "",
					_color,
					(int)_ll, _body + _offset);
		else
			dprintf(fd, _format_body,
					_indent, "",
					(int)_ll, _body + _offset);

		_offset += _ll;

		if (_body[_offset] == '\n')
			_offset++;
	}

	if (report->sub)
		_logs_print(report->sub, fd, depth + 1);

	return (0);
}
