/**
 * @file _parsing.c
 * @brief Internal TOML parser helpers.
*/
/* ----| Headers    |----- */
	/* Standard */
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

	/* Internal */
#include "_toml.h"

	/* External */
	//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static int	_toml_parse_error(
	const char *const	line,
	const char *const	file,
	const int			line_nb,
	const int			col,
	const int			errnum
)
{
	_toml_error_set(line, file, line_nb, col > 0 ? col : 1, errnum);
	return (errnum);
}

static int	_toml_col(
	const char *const line,
	const char *const pos
)
{
	if (!line || !pos || pos < line)
		return (1);
	return ((int)(pos - line) + 1);
}

static int	_toml_only_space(
	const char *str
)
{
	while (str && *str)
	{
		if (!isspace((unsigned char)*str))
			return (0);
		str++;
	}
	return (1);
}

static char	*_toml_trim(
	char *str
)
{
	char	*end;

	while (*str && isspace((unsigned char)*str))
		str++;

	end = str + strlen(str);
	while (end > str && isspace((unsigned char)end[-1]))
		end--;

	*end = '\0';
	return (str);
}

static void	_toml_strip_comment(
	char *const line
)
{
	int		in_quote = 0;
	char	quote = 0;
	size_t	i = 0;

	while (line[i])
	{
		if ((line[i] == '"' || line[i] == '\'') && (i == 0 || line[i - 1] != '\\'))
		{
			if (!in_quote)
			{
				in_quote = 1;
				quote = line[i];
			}
			else if (line[i] == quote)
				in_quote = 0;
		}

		if (!in_quote && line[i] == '#')
		{
			line[i] = '\0';
			return ;
		}

		i++;
	}
}

static int	_toml_validate_quoted(
	const char *const	value,
	int *const			col
)
{
	const char	quote = value[0];
	size_t		i = 1;

	while (value[i])
	{
		if (quote == '"' && value[i] == '\\')
		{
			i++;
			if (!value[i])
			{
				*col = (int)i + 1;
				return (TOML_ERROR_UNTERMINATED_STRING);
			}
			if (value[i] == 'u' || value[i] == 'U')
			{
				const size_t	digits = value[i] == 'u' ? 4 : 8;
				size_t			j = 1;

				while (j <= digits)
				{
					if (!isxdigit((unsigned char)value[i + j]))
					{
						*col = (int)(i + j + 1);
						return (TOML_ERROR_INVALID_ESCAPE);
					}
					j++;
				}
				i += digits;
			}
			else if (!strchr("btnfr\"\\", value[i]))
			{
				*col = (int)i + 1;
				return (TOML_ERROR_INVALID_ESCAPE);
			}
		}
		else if (value[i] == quote)
		{
			if (!_toml_only_space(value + i + 1))
			{
				*col = (int)i + 2;
				return (TOML_ERROR_INVALID_STRING);
			}
			return (TOML_ERROR_NONE);
		}
		i++;
	}
	*col = 1;
	return (TOML_ERROR_UNTERMINATED_STRING);
}

static char	*_toml_dup_unquoted(
	const char *value
)
{
	t_toml_str	result = {0};
	size_t		i;
	char		quote;

	value = _toml_trim((char *)value);
	quote = *value;
	if (quote != '"' && quote != '\'')
		return (setting->dup(value, strlen(value) + 1));

	i = 1;
	while (value[i] && value[i] != quote)
	{
		if (quote == '"' && value[i] == '\\' && value[i + 1])
		{
			i++;
			if (value[i] == 'n')
				_toml_str_append_char(&result, '\n');
			else if (value[i] == 't')
				_toml_str_append_char(&result, '\t');
			else
				_toml_str_append_char(&result, value[i]);
		}
		else
			_toml_str_append_char(&result, value[i]);
		i++;
	}
	return (result.content);
}

static inline int	_toml_value_is_bool(
	const char *const s
)
{
	if (s[0] == 'F' || s[0] == 'T' || s[0] == 'f' || s[0] == 't')
		return (!strcmp(s + 1, "rue") || !strcmp(s + 1, "alse"));
	else
		return (false);
}

static int	_toml_value_type(
	const char *value
)
{
	size_t	i;
	int		has_dot = 0;
	int		has_digit = 0;

	value = _toml_trim((char *)value);
	if (*value == '"' || *value == '\'')
		return (toml_tok_str);
	else if (*value == '[')
		return (toml_tok_array);
	else if (_toml_value_is_bool(value))
		return (toml_tok_bool);

	i = 0;
	while (value[i])
	{
		if (isdigit((unsigned char)value[i]))
			has_digit = 1;
		else if (value[i] == '.' || value[i] == 'e' || value[i] == 'E')
			has_dot = 1;
		else if (value[i] == '-' || value[i] == '+' || value[i] == '_')
			;
		else
			return ((strchr(value, '-') || strchr(value, ':'))
						? toml_tok_datetime : toml_tok_str);
		i++;
	}

	if (!has_digit)
		return (toml_tok_str);

	return (has_dot ? toml_tok_float : toml_tok_int);
}

static char	*_toml_join_path(
	const char *const prefix,
	const char *const key
)
{
	t_toml_str	result;

	memset(&result, 0, sizeof(result));
	if (prefix && *prefix)
	{
		_toml_str_append_n(&result, prefix, strlen(prefix));
		_toml_str_append_char(&result, '.');
	}
	_toml_str_append_n(&result, key, strlen(key));
	return (result.content);
}

static int	_toml_set_parsed_scalar(
	TOML **const toml,
	const char *const path,
	char *const value,
	const char *const line,
	const char *const file,
	const int line_nb,
	const int value_col
)
{
	char	*data = NULL;
	int		type;
	int		errnum = error_none;
	int		col;

	if (unlikely(!*value))
		return (_toml_parse_error(line, file, line_nb, value_col,
				TOML_ERROR_INVALID_VALUE));
	type = _toml_value_type(value);
	if (type == toml_tok_str && (*value == '"' || *value == '\''))
	{
		errnum = _toml_validate_quoted(value, &col);
		if (unlikely(errnum != TOML_ERROR_NONE))
			return (_toml_parse_error(line, file, line_nb,
					value_col + col - 1, errnum));
	}
	if (type == toml_tok_str)
		data = _toml_dup_unquoted(value);
	else
		data = setting->dup(value, strlen(value) + 1);

	if (unlikely(!data))
		return (_toml_parse_error(line, file, line_nb, value_col,
				TOML_ERROR_ALLOC_FAIL));

	errnum = _toml_set_field(toml, path, data, type);
	setting->free(data);
	if (unlikely(errnum != error_none))
		_toml_parse_error(line, file, line_nb, value_col, errnum);
	return (errnum);
}

static int	_toml_parse_array_item(
	TOML *const array,
	char *item,
	const char *const line,
	const char *const file,
	const int line_nb,
	const int item_col
)
{
	char	*data;
	int		type;
	TOML	*node;
	int		col;
	int		errnum;

	item = _toml_trim(item);
	if (unlikely(!*item))
		return (_toml_parse_error(line, file, line_nb, item_col,
				TOML_ERROR_INVALID_ARRAY));
	type = _toml_value_type(item);
	if (type == toml_tok_str && (*item == '"' || *item == '\''))
	{
		errnum = _toml_validate_quoted(item, &col);
		if (unlikely(errnum != TOML_ERROR_NONE))
			return (_toml_parse_error(line, file, line_nb,
					item_col + col - 1, errnum));
	}
	if (type == toml_tok_str)
		data = _toml_dup_unquoted(item);
	else
		data = setting->dup(item, strlen(item) + 1);

	if (unlikely(!data))
		return (_toml_parse_error(line, file, line_nb, item_col,
				TOML_ERROR_ALLOC_FAIL));

	node = _toml_new_content(NULL, type, data);
	if (unlikely(!node))
	{
		setting->free(data);
		return (_toml_parse_error(line, file, line_nb, item_col,
				TOML_ERROR_ALLOC_FAIL));
	}

	errnum = _toml_add_child(array, node);
	if (unlikely(errnum != error_none))
		_toml_parse_error(line, file, line_nb, item_col, errnum);
	return (errnum);
}

static int	_toml_set_parsed_array(
	TOML **const toml,
	const char *const path,
	char *value,
	const char *const line,
	const char *const file,
	const int line_nb,
	const int value_col
)
{
	TOML	*array;
	char	*line_copy;
	const char	*err_line;
	char	*start;
	char	*end = NULL;
	char	*item_start;
	size_t	i;
	int		in_quote = 0;
	char	quote = '\0';
	int		errnum;
	int		item_col;

	line_copy = setting->dup(line, strlen(line) + 1);
	err_line = line_copy ? line_copy : line;
	array = _toml_new_content(NULL, toml_tok_array, NULL);
	if (unlikely(!array))
	{
		setting->free(line_copy);
		return (_toml_parse_error(line, file, line_nb, value_col,
				TOML_ERROR_ALLOC_FAIL));
	}

	start = strchr(value, '[');
	end = strrchr(value, ']');
	if (unlikely(!start || !end || end < start))
	{
		_toml_free_content(array);
		setting->free(line_copy);
		return (_toml_parse_error(line, file, line_nb, value_col,
				TOML_ERROR_UNTERMINATED_ARRAY));
	}
	if (unlikely(!_toml_only_space(end + 1)))
	{
		_toml_free_content(array);
		setting->free(line_copy);
		return (_toml_parse_error(line, file, line_nb,
				value_col + (int)(end - value) + 1,
				TOML_ERROR_INVALID_ARRAY));
	}

	*end = '\0';
	start++;
	item_start = start;
	i = 0;
	while (start[i])
	{
		if ((start[i] == '"' || start[i] == '\'') && (i == 0 || start[i - 1] != '\\'))
		{
			if (!in_quote)
			{
				in_quote = 1;
				quote = start[i];
			}
			else if (start[i] == quote)
				in_quote = 0;
		}

		if (!in_quote && start[i] == ',')
		{
			start[i] = '\0';
			item_col = value_col + (int)(item_start - value);
			errnum = _toml_parse_array_item(array, item_start, err_line, file,
					line_nb, item_col);
			if (unlikely(errnum != error_none))
				goto cleanup;
			item_start = start + i + 1;
		}

		i++;
	}
	if (unlikely(in_quote))
	{
		errnum = _toml_parse_error(err_line, file, line_nb, value_col,
				TOML_ERROR_UNTERMINATED_STRING);
		goto cleanup;
	}

	item_start = _toml_trim(item_start);
	if (*item_start)
	{
		item_col = value_col + (int)(item_start - value);
		errnum = _toml_parse_array_item(array, item_start, err_line, file,
				line_nb, item_col);
		if (unlikely(errnum != error_none))
			goto cleanup;
	}

	errnum = _toml_set_node(toml, path, array, toml_tok_array);
	if (unlikely(errnum != error_none))
		_toml_parse_error(err_line, file, line_nb, value_col, errnum);

cleanup:
	_toml_free_content(array);
	setting->free(line_copy);
	return (errnum);
}

/* ----| Public     |----- */

int	_toml_parse_string(
	TOML **const toml,
	const char *const str,
	const char *const file
)
{
	char	*copy;
	char	*line;
	char	*next;
	char	*current_table;
	int		line_nb;
	int		errnum;

	if (unlikely(!toml || !str))
		return (_toml_parse_error(NULL, file, 0, 1, TOML_ERROR_INVALID_ARG));

	copy = setting->dup(str, strlen(str) + 1);
	if (unlikely(!copy))
		return (_toml_parse_error(NULL, file, 0, 1, TOML_ERROR_ALLOC_FAIL));

	_toml_error_set(NULL, NULL, 0, 0, TOML_ERROR_NONE);
	current_table = NULL;
	errnum = error_none;
	line = copy;
	line_nb = 1;
	while (line)
	{
		char	*trimmed;
		char	*eq;

		next = strchr(line, '\n');
		if (next)
			*next = '\0';
		_toml_strip_comment(line);
		trimmed = _toml_trim(line);
		if (!*trimmed)
		{
			line = next ? next + 1 : NULL;
			line_nb++;
			continue ;
		}

		if (*trimmed == '[')
		{
			char	*end = strchr(trimmed, ']');
			char	saved_end;

			if (!end)
			{
				errnum = _toml_parse_error(line, file, line_nb,
						_toml_col(line, trimmed),
						TOML_ERROR_INVALID_TABLE);
				break ;
			}
			if (unlikely(!_toml_only_space(end + 1)))
			{
				errnum = _toml_parse_error(line, file, line_nb,
						_toml_col(line, end + 1),
						TOML_ERROR_INVALID_TABLE);
				break ;
			}

			saved_end = *end;
			*end = '\0';
			setting->free(current_table);
			trimmed = _toml_trim(trimmed + 1);
			if (unlikely(!*trimmed))
			{
				*end = saved_end;
				errnum = _toml_parse_error(line, file, line_nb,
						_toml_col(line, trimmed),
						TOML_ERROR_INVALID_TABLE);
				current_table = NULL;
				break ;
			}
			current_table = setting->dup(trimmed, strlen(trimmed) + 1);
			if (unlikely(!current_table))
			{
				*end = saved_end;
				errnum = _toml_parse_error(line, file, line_nb,
						_toml_col(line, trimmed),
						TOML_ERROR_ALLOC_FAIL);
				break ;
			}

			if (unlikely(_toml_get_field(*toml, current_table, -1)))
			{
				*end = saved_end;
				errnum = _toml_parse_error(line, file, line_nb,
						_toml_col(line, trimmed),
						TOML_ERROR_DUPLICATE_TABLE);
				break ;
			}
			*end = saved_end;
			errnum = _toml_set_field(toml, current_table, NULL, toml_tok_table);
			if (unlikely(errnum != error_none))
			{
				_toml_parse_error(line, file, line_nb,
					_toml_col(line, trimmed), errnum);
				break ;
			}

			line = next ? next + 1 : NULL;
			line_nb++;
			continue ;
		}

		eq = strchr(trimmed, '=');
		if (!eq)
		{
			errnum = _toml_parse_error(line, file, line_nb,
					_toml_col(line, trimmed), TOML_ERROR_UNEXPECTED_TOKEN);
			break ;
		}

		*eq = '\0';
		{
			char	*key = _toml_trim(trimmed);
			char	*value = _toml_trim(eq + 1);
			char	*path = NULL;

			if (unlikely(!*key))
			{
				*eq = '=';
				errnum = _toml_parse_error(line, file, line_nb,
						_toml_col(line, trimmed),
						TOML_ERROR_EMPTY_KEY);
				break ;
			}
			if (unlikely(!*value))
			{
				*eq = '=';
				errnum = _toml_parse_error(line, file, line_nb,
						_toml_col(line, eq + 1),
						TOML_ERROR_INVALID_VALUE);
				break ;
			}
			path = _toml_join_path(current_table, key);
			if (unlikely(!path))
			{
				*eq = '=';
				errnum = _toml_parse_error(line, file, line_nb,
						_toml_col(line, key),
						TOML_ERROR_ALLOC_FAIL);
				break ;
			}
			if (unlikely(_toml_get_field(*toml, path, -1)))
			{
				*eq = '=';
				errnum = _toml_parse_error(line, file, line_nb,
						_toml_col(line, key),
						TOML_ERROR_DUPLICATE_KEY);
				setting->free(path);
				break ;
			}

			*eq = '=';
			if (_toml_value_type(value) == toml_tok_array)
				errnum = _toml_set_parsed_array(toml, path, value, line, file,
						line_nb, _toml_col(line, value));
			else
				errnum = _toml_set_parsed_scalar(toml, path, value, line, file,
						line_nb, _toml_col(line, value));

			setting->free(path);
			if (unlikely(errnum != error_none))
				break ;
		}

		line = next ? next + 1 : NULL;
		line_nb++;
	}

	setting->free(current_table);
	setting->free(copy);

	return (errnum);
}

TOML	*_toml_load_file(
	const char *const restrict path
)
{
	FILE	*file;
	long	size;
	char	*buffer;
	TOML	*result;

	file = fopen(path, "rb");
	if (unlikely(!file))
	{
		_toml_error_set(NULL, path, 0, 0, TOML_ERROR_INVALID_FILE);
		return (NULL);
	}

	if (fseek(file, 0, SEEK_END) != 0)
	{
		_toml_error_set(NULL, path, 0, 0, TOML_ERROR_SYSCALL_FAIL);
		fclose(file);
		return (NULL);
	}

	size = ftell(file);
	if (size < 0 || fseek(file, 0, SEEK_SET) != 0)
	{
		_toml_error_set(NULL, path, 0, 0, TOML_ERROR_SYSCALL_FAIL);
		fclose(file);
		return (NULL);
	}

	buffer = setting->alloc((size_t)size + 1);
	if (unlikely(!buffer))
	{
		_toml_error_set(NULL, path, 0, 0, TOML_ERROR_ALLOC_FAIL);
		fclose(file);
		return (NULL);
	}

	if (size && fread(buffer, 1, (size_t)size, file) != (size_t)size)
	{
		_toml_error_set(NULL, path, 0, 0, TOML_ERROR_SYSCALL_FAIL);
		setting->free(buffer);
		fclose(file);
		return (NULL);
	}

	fclose(file);
	buffer[size] = '\0';
	result = toml_new();
	if (unlikely(!result))
		_toml_error_set(NULL, path, 0, 0, TOML_ERROR_ALLOC_FAIL);
	else if (unlikely(_toml_parse_string(&result, buffer, path) != error_none))
	{
		toml_unload(result);
		result = NULL;
	}
	setting->free(buffer);
	return (result);
}

TOML	*_toml_load_str(
	const char *const restrict format,
	va_list *const args
)
{
	t_toml_str	str = {0};
	TOML		*result;

	if (unlikely(_toml_fill_format(format, &str, args) != error_none))
	{
		_toml_error_set(NULL, NULL, 0, 0, TOML_ERROR_ALLOC_FAIL);
		setting->free(str.content);
		return (NULL);
	}

	result = toml_new();
	if (unlikely(!result))
	{
		_toml_error_set(NULL, NULL, 0, 0, TOML_ERROR_ALLOC_FAIL);
		setting->free(str.content);
		return (NULL);
	}

	else if (unlikely(_toml_parse_string(&result, str.content, NULL) != error_none))
	{
		toml_unload(result);
		result = NULL;
	}

	setting->free(str.content);
	return (result);
}
