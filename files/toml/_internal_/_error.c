/**
 * @file _edit.c
 * @brief Internal TOML tree mutation helpers.
*/
/* ----| Headers    |----- */
	/* Standard */
#include <string.h>
#include <errno.h>

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

/* ----| Public     |----- */

t_toml_error	*_toml_error(void)
{
	static t_toml_error	error = {0};

	return (&error);
}

int	_toml_error_set(
	const char *const	line_str,
	const int			line,
	const int 			col,
	const int			errnum
)
{
	char	*str = NULL;
	
	if (line_str)
	{
		str = setting->dup(line_str, strlen(line_str));

		if (unlikely(!str))
		{
			_toml_error_set(NULL, line, col, -errno);
			return (-errno);
		}
	}

	*_toml_error() = (t_toml_error){
		.str_line = line_str,
		.line = line,
		.col = col,
		.errnum = errnum
	};

	return (error_none);
}

char	*_toml_error_get(void)
{
	const t_toml_error *const	error = _toml_error();
	char						format[1024] = {0};
	t_toml_str					str = {0};

	if (!error->errnum)
	{
		snprintf(format, sizeof(format), GREEN "Error: Success\n" RESET);

	}



	snprintf(format, sizeof(format), RED "Error: %%s\n")

	if (unlikely(!_create_error_str(&str, RED "Error: %s")))
}