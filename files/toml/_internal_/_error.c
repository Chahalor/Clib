/**
 * @file _edit.c
 * @brief Internal TOML tree mutation helpers.
*/
/* ----| Headers    |----- */
	/* Standard */
#include <string.h>
#include <errno.h>
#include <unistd.h>

	/* Internal */
#include "_toml.h"

	/* External */
#include "formating.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

int	_create_error_str(
	t_toml_str *const			dest,
	const char *const restrict	format,
	...
)
{
	va_list	args;
	int		result = error_none;

	va_start(args, format);
	result = _toml_fill_format(format, dest, &args);

	va_end(args);
	return (result);
}

static inline int	_dump_tty(
	FILE *const					file,
	const t_toml_error *const	error
)
{
	int	r = 0;

	if (unlikely(!error->errnum))
		return (fprintf(file, GREEN "Error: " RESET " Success\n"));

	if (error->errnum < 0)
	{
		if (error->str_line)
		{
			fprintf(file, GREEN "┌┤" RED "Error:" RESET " %s (%s:%d:%d)\n",
				strerror(-error->errnum), error->file ? error->file : "",
				error->line, error->col);

			size_t len = strlen(error->str_line);

			fprintf(file, GREEN "│" RESET " %s", error->str_line);
			fprintf(file, GREEN "│%*s%s\n", error->col, "", "─┬─");
			fprintf(file, "└");

			for (size_t i = 1; i < error->col; i++)
				fputs("─", file);
			fprintf(file, "──┘\n" RESET);
		}
		else
			fprintf(file, RED "Error:" RESET " %s (%s:%d:%d)\n",
				strerror(-error->errnum), error->file ? error->file : "",
				error->line, error->col);
	}
	else
	{
		if (error->str_line)
		{
			fprintf(file, GREEN "┌┤" RED "Error:" RESET " %s (%s:%d:%d)\n",
				_toml_error_str(-error->errnum), error->file ? error->file : "",
				error->line, error->col);

			size_t len = strlen(error->str_line);

			fprintf(file, GREEN "│" RESET " %s", error->str_line);
			fprintf(file, GREEN "│%*s%s\n", error->col, "", "─┬─");
			fprintf(file, "└");

			for (size_t i = 1; i < error->col; i++)
				fputs("─", file);
			fprintf(file, "──┘\n" RESET);
		}
		else
			fprintf(file, RED "Error:" RESET " %s (%s:%d:%d)\n",
				_toml_error_str(-error->errnum), error->file ? error->file : "",
				error->line, error->col);
	}

	return (r);
}

static inline int	_dump(
	FILE *const					file,
	const t_toml_error *const	error
)
{
	int	r = 0;

	if (unlikely(!error->errnum))
		return (fprintf(file, "Error: Success\n"));

	if (error->errnum < 0)
	{
		if (error->str_line)
		{
			fprintf(file, "┌┤Error: %s (%s:%d:%d)\n",
				strerror(-error->errnum), error->file ? error->file : "",
				error->line, error->col);

			size_t len = strlen(error->str_line);

			fprintf(file, "│ %s", error->str_line);
			fprintf(file, "│%*s%s\n", error->col, "", "─┬─");
			fprintf(file, "└");

			for (size_t i = 1; i < error->col; i++)
				fputs("─", file);
			fprintf(file, "──┘\n");
		}
		else
			fprintf(file, "Error: %s (%s:%d:%d)\n",
				strerror(-error->errnum), error->file ? error->file : "",
				error->line, error->col);
	}
	else
	{
		if (error->str_line)
		{
			fprintf(file, "┌┤Error: %s (%s:%d:%d)\n",
				_toml_error_str(-error->errnum), error->file ? error->file : "",
				error->line, error->col);

			size_t len = strlen(error->str_line);

			fprintf(file, "│ %s", error->str_line);
			fprintf(file, "│%*s%s\n", error->col, "", "─┬─");
			fprintf(file, "└");

			for (size_t i = 1; i < error->col; i++)
				fputs("─", file);
			fprintf(file, "──┘\n");
		}
		else
			fprintf(file, "Error: %s (%s:%d:%d)\n",
				_toml_error_str(-error->errnum), error->file ? error->file : "",
				error->line, error->col);
	}

	return (r);
}

/* ----| Public     |----- */

__attribute__((destructor))
void	_toml_error_free(void)
{
	t_toml_error *const	error = _toml_error();

	mem_free(error->str_line);
	mem_free(error->file);
}

t_toml_error	*_toml_error(void)
{
	static t_toml_error	error = {0};

	return (&error);
}

int	_toml_error_set(
	const char *const	line_str,
	const char *const	file,
	const int			line,
	const int			col,
	const int			errnum
)
{
	t_toml_error *const	error = _toml_error();
	char				*str = NULL;
	char				*_file = NULL;

	if (error->str_line)
		mem_free(error->str_line);

	if (line_str)
	{
		str = setting->dup(line_str, strlen(line_str));

		if (unlikely(!str))
		{
			_toml_error_set(NULL, file, line, col, -errno);
			return (-errno);
		}
	}

	if (file)
	{
		_file = setting->dup(file, strlen(file));

		if (unlikely(!_file))
		{
			_toml_error_set(NULL, NULL, line, col, -errno);
			return (-errno);
		}
	}

	*error = (t_toml_error){
		.str_line = str,
		.file = _file,
		.line = line,
		.col = col,
		.errnum = errnum
	};

	return (error_none);
}

const char	*_toml_error_str(
	const enum e_toml_errors	errnum
)
{
	static const char	*strs[] = {
		[TOML_ERROR_NONE] = "Success"
	};
	static char			fallback[32] = {0};

	if (unlikely(errnum < 0))
	{
		snprintf(fallback, sizeof(fallback), "Unknow error (%d)", errnum);
		return (fallback);
	}

	return (strs[errnum]);
}

void	_toml_perror(
	const char *const s
)
{
	char	format[4096] = {0};

	snprintf(format, sizeof(format), "%s: %%s\n", s);
	fprintf(stderr, format, _toml_error_str(_toml_error()->errnum));
}

void	_toml_error_dump(
	FILE *const file
)
{
	const t_toml_error *const	error = _toml_error();

	if (isatty(fileno(file)))
		_dump_tty(file, error);
	else
		_dump(file, error);
}