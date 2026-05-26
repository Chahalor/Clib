/**
 * @file _parsing.c
 * @brief Internal TOML parser helpers.
 */
/* ----| Headers    |----- */
	/* Standard */
#include <ctype.h>
#include <string.h>

	/* Internal */
#include "_toml.h"

	/* External */
	//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

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
		return (mem_dup(value, strlen(value) + 1));

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
	char *const value
)
{
	char	*data = NULL;
	int		type;
	int		errnum = error_none;

	type = _toml_value_type(value);
	if (type == toml_tok_str)
		data = _toml_dup_unquoted(value);
	else
		data = mem_dup(value, strlen(value) + 1);

	if (unlikely(!data))
		return (error_alloc_fail);

	errnum = _toml_set_field(toml, path, data, type);
	mem_free(data);
	return (errnum);
}

static int	_toml_parse_array_item(
	TOML *const array,
	char *item
)
{
	char	*data;
	int		type;
	TOML	*node;

	item = _toml_trim(item);
	type = _toml_value_type(item);
	if (type == toml_tok_str)
		data = _toml_dup_unquoted(item);
	else
		data = mem_dup(item, strlen(item) + 1);

	if (unlikely(!data))
		return (error_alloc_fail);

	node = _toml_new_content(NULL, type, data);
	if (unlikely(!node))
	{
		mem_free(data);
		return (error_alloc_fail);
	}

	return (_toml_add_child(array, node));
}

static int	_toml_set_parsed_array(
	TOML **const toml,
	const char *const path,
	char *value
)
{
	TOML	*array;
	char	*start;
	char	*end = '\0';
	char	*item_start;
	size_t	i;
	int		in_quote = 0;
	char	quote = '\0';
	int		errnum;

	array = _toml_new_content(NULL, toml_tok_array, NULL);
	if (unlikely(!array))
		return (error_alloc_fail);

	start = strchr(value, '[');
	end = strrchr(value, ']');
	if (unlikely(!start || !end || end < start))
	{
		_toml_free_content(array);
		return (error_invalid_arg);
	}

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
			errnum = _toml_parse_array_item(array, item_start);
			if (unlikely(errnum != error_none))
				goto cleanup;
			item_start = start + i + 1;
		}

		i++;
	}

	item_start = _toml_trim(item_start);
	if (*item_start)
	{
		errnum = _toml_parse_array_item(array, item_start);
		if (unlikely(errnum != error_none))
			goto cleanup;
	}

	errnum = _toml_set_node(toml, path, array, toml_tok_array);

cleanup:
	_toml_free_content(array);
	return (errnum);
}

/* ----| Public     |----- */

int	_toml_parse_string(
	TOML **const toml,
	const char *const str
)
{
	char	*copy;
	char	*line;
	char	*current_table;
	int		errnum;

	if (unlikely(!toml || !str))
		return (error_invalid_arg);

	copy = mem_dup(str, strlen(str) + 1);
	if (unlikely(!copy))
		return (error_alloc_fail);

	current_table = NULL;
	errnum = error_none;
	line = strtok(copy, "\n");
	while (line)
	{
		char	*trimmed;
		char	*eq;

		_toml_strip_comment(line);
		trimmed = _toml_trim(line);
		if (!*trimmed)
		{
			line = strtok(NULL, "\n");
			continue ;
		}

		if (*trimmed == '[')
		{
			char	*end = strchr(trimmed, ']');

			if (!end)
			{
				errnum = error_invalid_arg;
				break ;
			}

			*end = '\0';
			mem_free(current_table);
			current_table = mem_dup(_toml_trim(trimmed + 1), strlen(_toml_trim(trimmed + 1)) + 1);
			if (unlikely(!current_table))
			{
				errnum = error_alloc_fail;
				break ;
			}

			errnum = _toml_set_field(toml, current_table, NULL, toml_tok_table);
			if (unlikely(errnum != error_none))
				break ;

			line = strtok(NULL, "\n");
			continue ;
		}

		eq = strchr(trimmed, '=');
		if (!eq)
		{
			line = strtok(NULL, "\n");
			continue ;
		}

		*eq = '\0';
		{
			char	*key = _toml_trim(trimmed);
			char	*value = _toml_trim(eq + 1);
			char	*path = _toml_join_path(current_table, key);

			if (unlikely(!path))
			{
				errnum = error_alloc_fail;
				break ;
			}

			if (_toml_value_type(value) == toml_tok_array)
				errnum = _toml_set_parsed_array(toml, path, value);
			else
				errnum = _toml_set_parsed_scalar(toml, path, value);

			mem_free(path);
			if (unlikely(errnum != error_none))
				break ;
		}

		line = strtok(NULL, "\n");
	}

	mem_free(current_table);
	mem_free(copy);
	return (errnum);
}
