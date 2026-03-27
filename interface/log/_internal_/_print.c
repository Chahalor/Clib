// Header

/* ----| Headers    |----- */
	/* Standard */
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

	/* Internal */
#include "_log.h"

	/* External */
#include "formating.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

// [year-mouth-day hour:minute:second] <ERROR> <function> (file:line)  code=<code> - <Logs summary>
#ifdef DEBUG
# define	FORMAT_SUM_TTY	GREY "%s" RESET " - %s%s" RESET " - %s - " GREY "(%s:%d)" RESET " code=%d - %s\n"
# define	FORMAT_SUM_FILE	     "%s - "         "%s"       " - %s - "      "(%s:%d)"       " code=%d - %s\n"
#else
# define	FORMAT_SUM_TTY	GREY "%s" RESET " - %s%s" RESET" code=%d - %s\n"
# define	FORMAT_SUM_FILE	     "%s - "         "%s"      " code=%d - %s\n"
#endif	// DEBUG

# define FORMAT_BODY_TTY  " %*s%s|" RESET " %.*s\n"	//	"(depth * \t)|<line x>"
# define FORMAT_BODY_FILE " %*s|"         " %.*s\n"	//	"(depth * \t)|<line x>"

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

static size_t	_append_log(
	const t_log_report *report,
	char *buff,
	size_t cap,
	size_t len,
	int depth,
	bool tty
)
{
	if (!report || len >= cap)
		return (len);

	const char	*_color = _log_level_color(report->level);
	const char	*_func = report->func ? report->func : "(unknown)";
	const char	*_file = report->file ? report->file : "(unknown)";
	const char	*_summary = report->summary ? report->summary : "";
	const char	*_body = report->body ? report->body : "";

	const char	*_format_sum = tty ? FORMAT_SUM_TTY : FORMAT_SUM_FILE;
	const char	*_format_body = tty ? FORMAT_BODY_TTY : FORMAT_BODY_FILE;

	time_t		now = time(NULL);
	struct tm	tm;
	localtime_r(&now, &tm);

	char	timebuf[64];
	strftime(timebuf, sizeof(timebuf), "[%Y-%m-%d %H:%M:%S]", &tm);

	// indentation prefix
	if (depth)
		len += snprintf(buff + len, cap - len,
				"%*s\\_", (depth * 4) - 2, "");

	// summary
#ifdef DEBUG
	if (tty)
		len += snprintf(buff + len, cap - len,
				_format_sum,
				timebuf, _color,
				_log_level_str(report->level),
				_func, _file,
				report->line, report->code,
				_summary);
	else
		len += snprintf(buff + len, cap - len,
				_format_sum,
				timebuf,
				_log_level_str(report->level),
				_func, _file,
				report->line, report->code,
				_summary);
#else
	if (tty)
		len += snprintf(buff + len, cap - len,
				_format_sum,
				timebuf, _color,
				_log_level_str(report->level),
				report->code,
				_summary);
	else
		len += snprintf(buff + len, cap - len,
				_format_sum,
				timebuf,
				_log_level_str(report->level),
				report->code,
				_summary);
#endif

	// body
	size_t offset = 0;
	while (_body[offset] && len < cap)
	{
		size_t ll = _line_len(_body + offset);
		int indent = depth * 4;

		if (tty)
			len += snprintf(buff + len, cap - len,
					_format_body,
					indent, "",
					_color,
					(int)ll, _body + offset);
		else
			len += snprintf(buff + len, cap - len,
					_format_body,
					indent, "",
					(int)ll, _body + offset);

		offset += ll;
		if (_body[offset] == '\n')
			offset++;
	}

	// recurse INTO BUFFER (not output)
	if (report->sub)
		len = _append_log(report->sub, buff, cap, len, depth + 1, tty);

	return (len);
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

	char		buff[4096];
	size_t		len = 0;

	const bool	_tty = isatty(fd);
	const char	*_color = _log_level_color(report->level);
	const char	*_func = report->func ? report->func : "(unknown)";
	const char	*_file = report->file ? report->file : "(unknown)";
	const char	*_summary = report->summary ? report->summary : "";
	const char	*_body = report->body ? report->body : "";

	time_t		now = time(NULL);
	struct tm	tm;
	localtime_r(&now, &tm);

	char		timebuf[64];
	strftime(timebuf, sizeof(timebuf), "[%Y-%m-%d %H:%M:%S]", &tm);

	const char	*_format_sum = _tty ? FORMAT_SUM_TTY : FORMAT_SUM_FILE;
	const char	*_format_body = _tty ? FORMAT_BODY_TTY : FORMAT_BODY_FILE;

	// prefix indentation
	if (depth)
		len += snprintf(buff + len, sizeof(buff) - len,
				"%*s\\_", (depth * 4) - 2, "");

	// summary
#ifdef DEBUG
	if (_tty)
		len += snprintf(buff + len, sizeof(buff) - len,
				_format_sum,
				timebuf, _color, _log_level_str(report->level),
				_func, _file, report->line, report->code, _summary);
	else
		len += snprintf(buff + len, sizeof(buff) - len,
				_format_sum,
				timebuf, _log_level_str(report->level),
				_func, _file, report->line, report->code, _summary);
#else
	if (_tty)
		len += snprintf(buff + len, sizeof(buff) - len,
				_format_sum,
				timebuf, _color,
				_log_level_str(report->level),
				report->code, _summary);
	else
		len += snprintf(buff + len, sizeof(buff) - len,
				_format_sum,
				timebuf,
				_log_level_str(report->level),
				report->code, _summary);
#endif

	// body
	size_t offset = 0;
	while (_body[offset] && len < sizeof(buff))
	{
		size_t ll = _line_len(_body + offset);
		int indent = depth * 4;

		if (_tty)
			len += snprintf(buff + len, sizeof(buff) - len,
					_format_body,
					indent, "",
					_color,
					(int)ll, _body + offset);
		else
			len += snprintf(buff + len, sizeof(buff) - len,
					_format_body,
					indent, "",
					(int)ll, _body + offset);

		offset += ll;
		if (_body[offset] == '\n')
			offset++;
	}

	ssize_t	_n;
	size_t	_offset = 0;
	while (len - _offset > 0 && (_n = write(fd, buff + _offset, len - _offset)) > 0)
	{
		_offset += _n;
	}

	if (report->sub)
		_logs_print(report->sub, fd, depth + 1);

	return (0);
}

int	_logs_print_atomic(
	const t_log_report *report,
	const int fd
)
{
	if (!report)
		return (0);

	char	buff[LOG_BUFFER_SIZE];
	size_t	len = 0;
	bool	tty = isatty(fd);
	ssize_t	_n;
	size_t	_offset = 0;

	len = _append_log(report, buff, sizeof(buff), len, 0, tty);

	if (len > sizeof(buff))
		len = sizeof(buff);

	while (len - _offset > 0 && (_n = write(fd, buff + _offset, len - _offset)) > 0)
	{
		_offset += _n;
	}

	return (0);
}