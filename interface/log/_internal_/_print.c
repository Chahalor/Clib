/**
 * @file _print.c
 * @brief Internal formatting and output routines for `interface/log`.
 */

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

/**
 * @brief Map a log level to its printable short string.
 *
 * @param level Input log level.
 *
 * @return Constant level label (`ERROR`, `WARN`, `INFO`, `DEBUG`, `OTHER`).
 *
 * @code{.c}
 * const char *label = _log_level_str(log_warning); // "WARN"
 * @endcode
 */
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

/**
 * @brief Map a log level to a terminal color escape sequence.
 *
 * @param level Input log level.
 *
 * @return ANSI color string for tty output, empty string for `log_other`.
 *
 * @code{.c}
 * const char *color = _log_level_color(log_error); // RED
 * @endcode
 */
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

/**
 * @brief Return the length of the next line fragment in `src`.
 *
 * Counts bytes until `'\0'` or `'\n'`.
 *
 * @param src Source string cursor.
 *
 * @return Number of bytes in the current line segment.
 *
 * @code{.c}
 * size_t n = _line_len("hello\nworld"); // 5
 * @endcode
 */
size_t	_line_len(
	const char *const src
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

/**
 * @brief Append one report tree into `buff`.
 *
 * This helper formats summary + body for `report`, then recurses into
 * `report->sub` with incremented indentation depth.
 *
 * @param report Report tree node.
 * @param buff Destination buffer.
 * @param cap Buffer capacity.
 * @param len Existing used length.
 * @param depth Tree depth.
 * @param tty Non-zero for terminal color formatting.
 *
 * @return New buffer length after append attempts.
 *
 * @code{.c}
 * char buf[LOG_BUFFER_SIZE];
 * size_t len = _append_log(&report, buf, sizeof(buf), 0, 0, true);
 * @endcode
 */
static size_t	_append_log(
	const t_log_report *const report,
	char *const buff,
	size_t cap,
	size_t len,
	int depth,
	bool tty
)
{
	if (len >= cap)
		return (len);

	const char	*_color = _log_level_color(report->level);
	const char	*_func = report->func		? report->func		: "(unknown)";
	const char	*_file = report->file		? report->file		: "(unknown)";
	const char	*_summary = report->summary	? report->summary	: "";
	const char	*_body = report->body		? report->body		: "";
	const char	*_format_sum = tty			? FORMAT_SUM_TTY	: FORMAT_SUM_FILE;
	const char	*_format_body = tty			? FORMAT_BODY_TTY	: FORMAT_BODY_FILE;
	time_t		now = report->time			? report->time		: time(NULL);
	struct tm	tm;
	char		timebuf[64];
	size_t		offset = 0;

	localtime_r(&now, &tm);
	strftime(timebuf, sizeof(timebuf), "[%Y-%m-%d %H:%M:%S]", &tm);

	if (depth)
		len += snprintf(buff + len, cap - len,
				"%*s\\_", (depth * 4) - 2, "");

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

	while (_body[offset] && len < cap)
	{
		size_t	_ll = _line_len(_body + offset);
		int		_indent = depth * 4;

		if (tty)
			len += snprintf(buff + len, cap - len,
					_format_body,
					_indent, "",
					_color,
					(int)_ll, _body + offset);
		else
			len += snprintf(buff + len, cap - len,
					_format_body,
					_indent, "",
					(int)_ll, _body + offset);

		offset += _ll;
		if (_body[offset] == '\n')
			offset++;
	}

	if (report->sub)
		len = _append_log(report->sub, buff, cap, len, depth + 1, tty);

	return (len);
}

/* ----| Public     |----- */

/**
 * @brief Print one report tree directly to an fd with incremental writes.
 *
 * @param report Report root.
 * @param fd Destination file descriptor.
 * @param depth Initial depth offset.
 *
 * @return `0` in current implementation.
 *
 * @code{.c}
 * _logs_print(&report, STDERR_FILENO, 0);
 * @endcode
 */
int	_logs_print(
	const t_log_report *const report,
	const int fd,
	const int depth
)
{
	const bool	_tty = isatty(fd);
	const char	*_color = _log_level_color(report->level);
	const char	*_format_sum = _tty			? FORMAT_SUM_TTY	: FORMAT_SUM_FILE;
	const char	*_format_body = _tty		? FORMAT_BODY_TTY	: FORMAT_BODY_FILE;
	const char	*_func = report->func		? report->func		: "(unknown)";
	const char	*_file = report->file		? report->file		: "(unknown)";
	const char	*_summary = report->summary	? report->summary	: "";
	const char	*_body = report->body		? report->body		: "";
	time_t		now = report->time			? report->time		: time(NULL);
	struct tm	tm;
	char		timebuf[64];
	char		buff[4096];
	size_t		len = 0;
	size_t		offset = 0;
	ssize_t		_n;
	size_t		_offset = 0;

	localtime_r(&now, &tm);
	strftime(timebuf, sizeof(timebuf), "[%Y-%m-%d %H:%M:%S]", &tm);

	if (depth)
		len += snprintf(buff + len, sizeof(buff) - len,
				"%*s\\_", (depth * 4) - 2, "");

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

	while (len - _offset > 0 && (_n = write(fd, buff + _offset, len - _offset)) > 0)
		_offset += _n;

	if (report->sub)
		_logs_print(report->sub, fd, depth + 1);

	return (0);
}

/**
 * @brief Print one report tree through a bounded single-buffer path.
 *
 * @param report Report root.
 * @param fd Destination file descriptor.
 *
 * @return `0` in current implementation.
 *
 * @code{.c}
 * _logs_print_atomic(&report, fd);
 * @endcode
 */
int	_logs_print_atomic(
	const t_log_report *const report,
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
