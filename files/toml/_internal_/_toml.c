#include "_toml.h"

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int	_toml_is_sep(const char c)
{
	return (c == '.' || c == '/');
}

static int	_toml_str_reserve(
	t_toml_str *const dest,
	const size_t add
)
{
	char	*new_content;
	size_t	new_size;

	if (unlikely(!dest))
		return (error_invalid_arg);
	if (dest->len + add + 1 <= dest->size)
		return (error_none);
	new_size = dest->size ? dest->size : INTERFACE_JSON_STRING_ALLOC_SIZE;
	while (dest->len + add + 1 > new_size)
		new_size *= 2;
	new_content = mem_alloc(new_size);
	if (unlikely(!new_content))
		return (error_alloc_fail);
	if (dest->content)
		memcpy(new_content, dest->content, dest->len);
	mem_free(dest->content);
	dest->content = new_content;
	dest->size = new_size;
	dest->content[dest->len] = '\0';
	return (error_none);
}

int	_toml_str_append_n(
	t_toml_str *const dest,
	const char *const src,
	const size_t len
)
{
	int	errnum;

	if (unlikely(!dest || (!src && len)))
		return (error_invalid_arg);
	errnum = _toml_str_reserve(dest, len);
	if (unlikely(errnum != error_none))
		return (errnum);
	if (len)
		memcpy(dest->content + dest->len, src, len);
	dest->len += len;
	dest->content[dest->len] = '\0';
	return (error_none);
}

int	_toml_str_append_char(
	t_toml_str *const dest,
	const char c
)
{
	return (_toml_str_append_n(dest, &c, 1));
}

static int	_toml_append_uint_base(
	t_toml_str *const dest,
	unsigned long long value,
	const unsigned int base
)
{
	char	buf[32];
	size_t	i;

	if (unlikely(!dest || base < 2 || base > 16))
		return (error_invalid_arg);
	i = sizeof(buf);
	if (!value)
		buf[--i] = '0';
	while (value)
	{
		buf[--i] = "0123456789abcdef"[value % base];
		value /= base;
	}
	return (_toml_str_append_n(dest, buf + i, sizeof(buf) - i));
}

static int	_toml_append_int(
	t_toml_str *const dest,
	long long value
)
{
	if (value < 0)
	{
		const int	errnum = _toml_str_append_char(dest, '-');

		if (unlikely(errnum != error_none))
			return (errnum);
		return (_toml_append_uint_base(dest, -(unsigned long long)value, 10));
	}
	return (_toml_append_uint_base(dest, (unsigned long long)value, 10));
}

int	_toml_fill_format(
	const char *const str,
	t_toml_str *const result,
	va_list *const args
)
{
	size_t	i;
	int		errnum;

	if (unlikely(!str || !result || !args))
		return (error_invalid_arg);
	i = 0;
	while (str[i])
	{
		if (str[i] != '%')
		{
			errnum = _toml_str_append_char(result, str[i++]);
			if (unlikely(errnum != error_none))
				return (errnum);
			continue ;
		}
		if (!str[i + 1])
			return (_toml_str_append_char(result, '%'));
		if (str[i + 1] == '%')
		{
			errnum = _toml_str_append_char(result, '%');
			if (unlikely(errnum != error_none))
				return (errnum);
			i += 2;
			continue ;
		}
		if (str[i + 1] == 'l' && (str[i + 2] == 'd' || str[i + 2] == 'i'))
		{
			errnum = _toml_append_int(result, va_arg(*args, long));
			i += 3;
		}
		else if (str[i + 1] == 'l' && str[i + 2] == 'x')
		{
			errnum = _toml_append_uint_base(result,
					va_arg(*args, unsigned long), 16);
			i += 3;
		}
		else if (str[i + 1] == 'd' || str[i + 1] == 'i')
		{
			errnum = _toml_append_int(result, va_arg(*args, int));
			i += 2;
		}
		else if (str[i + 1] == 's')
		{
			const char	*value = va_arg(*args, char *);

			errnum = _toml_str_append_n(result, value ? value : "(null)",
					strlen(value ? value : "(null)"));
			i += 2;
		}
		else if (str[i + 1] == 'p')
		{
			errnum = _toml_str_append_n(result, "0x", 2);
			if (unlikely(errnum != error_none))
				return (errnum);
			errnum = _toml_append_uint_base(result,
					(unsigned long long)(uintptr_t)va_arg(*args, void *), 16);
			i += 2;
		}
		else if (str[i + 1] == 'x')
		{
			errnum = _toml_append_uint_base(result,
					va_arg(*args, unsigned int), 16);
			i += 2;
		}
		else if (str[i + 1] == 'c')
		{
			const char	c = (char)va_arg(*args, int);

			errnum = _toml_str_append_char(result, c);
			i += 2;
		}
		else
		{
			errnum = _toml_str_append_char(result, '%');
			if (unlikely(errnum != error_none))
				return (errnum);
			errnum = _toml_str_append_char(result, str[i + 1]);
			i += 2;
		}
		if (unlikely(errnum != error_none))
			return (errnum);
	}
	return (error_none);
}

TOML	*_toml_new_content(
	const char *const key,
	const int type,
	char *const data
)
{
	TOML	*result;

	result = mem_alloc(sizeof(TOML));
	if (unlikely(!result))
		return (NULL);
	memset(result, 0, sizeof(TOML));
	if (key)
	{
		result->key = mem_dup(key, strlen(key) + 1);
		if (unlikely(!result->key))
		{
			mem_free(result);
			return (NULL);
		}
	}
	result->type = type;
	result->data = data;
	return (result);
}

void	_toml_free_content(
	TOML *node
)
{
	if (!node)
		return ;
	mem_free(node->key);
	mem_free(node->data);
	_toml_free_content(node->child);
	_toml_free_content(node->next);
	mem_free(node);
}

TOML	*_toml_clone_node(
	const TOML *const src
)
{
	TOML	*node;
	char	*data;

	if (!src)
		return (NULL);
	data = NULL;
	if (src->data)
	{
		data = mem_dup(src->data, strlen(src->data) + 1);
		if (unlikely(!data))
			return (NULL);
	}
	node = _toml_new_content(src->key, src->type, data);
	if (unlikely(!node))
	{
		mem_free(data);
		return (NULL);
	}
	if (src->child)
	{
		node->child = _toml_clone_node(src->child);
		if (unlikely(!node->child))
			goto error;
	}
	if (src->next)
	{
		node->next = _toml_clone_node(src->next);
		if (unlikely(!node->next))
			goto error;
	}
	return (node);

error:
	_toml_free_content(node);
	return (NULL);
}

int	_toml_count_field(
	const char *const field
)
{
	size_t	i;
	int		count;
	int		in_segment;

	if (!field)
		return (0);
	i = 0;
	count = 0;
	in_segment = 0;
	while (field[i])
	{
		if (!_toml_is_sep(field[i]))
		{
			if (!in_segment)
				count++;
			in_segment = 1;
		}
		else
			in_segment = 0;
		i++;
	}
	return (count);
}

char	**_toml_split(
	const char *const field
)
{
	char	**result;
	char	*zone;
	size_t	len;
	size_t	i;
	size_t	part;
	size_t	zone_offset;
	int		count;

	if (unlikely(!field))
		return (NULL);
	len = strlen(field);
	count = _toml_count_field(field);
	result = mem_alloc(sizeof(char *) * (count + 1) + len + count + 1);
	if (unlikely(!result))
		return (NULL);
	zone = (char *)(result + count + 1);
	i = 0;
	part = 0;
	zone_offset = 0;
	while (field[i])
	{
		size_t	start;
		size_t	part_len;

		while (field[i] && _toml_is_sep(field[i]))
			i++;
		if (!field[i])
			break ;
		start = i;
		while (field[i] && !_toml_is_sep(field[i]))
			i++;
		part_len = i - start;
		result[part] = zone + zone_offset;
		memcpy(result[part], field + start, part_len);
		result[part][part_len] = '\0';
		zone_offset += part_len + 1;
		part++;
	}
	result[part] = NULL;
	return (result);
}

static size_t	_toml_split_len(char **split)
{
	size_t	len;

	len = 0;
	while (split && split[len])
		len++;
	return (len);
}

static TOML	*_toml_find_child(
	TOML *const parent,
	const char *const key
)
{
	TOML	*node;

	if (unlikely(!parent || !key))
		return (NULL);
	node = parent->child;
	while (node)
	{
		if (node->key && strcmp(node->key, key) == 0)
			return (node);
		node = node->next;
	}
	return (NULL);
}

static TOML	*_toml_get_array_index(
	TOML *node,
	const int index
)
{
	int	i;

	i = 0;
	while (node && i < index)
	{
		node = node->next;
		i++;
	}
	return (node);
}

int	_toml_add_child(
	TOML *const parent,
	TOML *const child
)
{
	TOML	*node;

	if (unlikely(!parent || !child))
		return (error_invalid_arg);
	if (!parent->child)
	{
		parent->child = child;
		return (error_none);
	}
	node = parent->child;
	while (node->next)
		node = node->next;
	node->next = child;
	return (error_none);
}

TOML	*_toml_get_field(
	TOML *const toml,
	const char *const field,
	const int depth
)
{
	char	**split;
	TOML	*current;
	size_t	limit;
	size_t	i;

	if (unlikely(!toml || !field))
		return (NULL);
	split = _toml_split(field);
	if (unlikely(!split))
		return (NULL);
	limit = depth < 0 ? _toml_split_len(split) : (size_t)depth;
	current = toml;
	i = 0;
	while (current && i < limit)
	{
		if (current->type == toml_tok_array)
		{
			current = _toml_get_array_index(current->child, atoi(split[i]));
			i++;
		}
		else
		{
			current = _toml_find_child(current, split[i]);
			i++;
		}
	}
	mem_free(split);
	return (current);
}

static int	_toml_replace_node_value(
	TOML *const target,
	const char *const data,
	const int type
)
{
	char	*new_data;

	if (unlikely(!target))
		return (error_invalid_arg);
	new_data = NULL;
	if (data)
	{
		new_data = mem_dup(data, strlen(data) + 1);
		if (unlikely(!new_data))
			return (error_alloc_fail);
	}
	if (type != toml_tok_array && type != toml_tok_table)
	{
		_toml_free_content(target->child);
		target->child = NULL;
	}
	mem_free(target->data);
	target->data = new_data;
	target->type = type;
	return (error_none);
}

int	_toml_set_field(
	TOML **const toml,
	const char *const field,
	const char *const data,
	const int type
)
{
	char	**split;
	TOML	*current;
	TOML	*child;
	size_t	i;
	size_t	len;
	int		errnum;

	if (unlikely(!toml || !field))
		return (error_invalid_arg);
	if (!*toml)
	{
		*toml = _toml_new_content(NULL, toml_tok_table, NULL);
		if (unlikely(!*toml))
			return (error_alloc_fail);
	}
	split = _toml_split(field);
	if (unlikely(!split))
		return (error_alloc_fail);
	len = _toml_split_len(split);
	current = *toml;
	i = 0;
	while (i < len)
	{
		child = _toml_find_child(current, split[i]);
		if (!child)
		{
			child = _toml_new_content(split[i],
					i + 1 == len ? type : toml_tok_table, NULL);
			if (unlikely(!child))
			{
				mem_free(split);
				return (error_alloc_fail);
			}
			errnum = _toml_add_child(current, child);
			if (unlikely(errnum != error_none))
			{
				_toml_free_content(child);
				mem_free(split);
				return (errnum);
			}
		}
		else if (i + 1 < len && child->type != toml_tok_table)
		{
			mem_free(child->data);
			child->data = NULL;
			_toml_free_content(child->child);
			child->child = NULL;
			child->type = toml_tok_table;
		}
		current = child;
		i++;
	}
	mem_free(split);
	return (_toml_replace_node_value(current, data, type));
}

int	_toml_set_node(
	TOML **const toml,
	const char *const field,
	const TOML *const value,
	const int type
)
{
	TOML	*target;
	TOML	*clone;
	int		errnum;

	if (unlikely(!toml || !field))
		return (error_invalid_arg);
	if (value && value->type != type)
		return (error_invalid_arg);
	errnum = _toml_set_field(toml, field, NULL, type);
	if (unlikely(errnum != error_none))
		return (errnum);
	target = _toml_get_field(*toml, field, -1);
	if (unlikely(!target))
		return (error_invalid_arg);
	_toml_free_content(target->child);
	target->child = NULL;
	if (!value || !value->child)
		return (error_none);
	clone = _toml_clone_node(value->child);
	if (unlikely(!clone))
		return (error_alloc_fail);
	target->child = clone;
	return (error_none);
}

static char	*_toml_format_field(
	const char *const field,
	va_list *const args
)
{
	t_toml_str	result;

	memset(&result, 0, sizeof(result));
	if (unlikely(_toml_fill_format(field, &result, args) != error_none))
	{
		mem_free(result.content);
		return (NULL);
	}
	return (result.content);
}

static int	_toml_detach_child(
	TOML *const parent,
	TOML *const target,
	const int should_free
)
{
	TOML	*node;
	TOML	*prev;

	if (unlikely(!parent || !target))
		return (error_invalid_arg);
	node = parent->child;
	prev = NULL;
	while (node && node != target)
	{
		prev = node;
		node = node->next;
	}
	if (!node)
		return (error_none);
	if (prev)
		prev->next = node->next;
	else
		parent->child = node->next;
	node->next = NULL;
	if (should_free)
		_toml_free_content(node);
	return (error_none);
}

int	_toml_unset(
	TOML **const toml,
	const char *field,
	const int should_free,
	va_list *const args
)
{
	char	*field_buf;
	TOML	*target;
	TOML	*parent;
	int		depth;
	int		errnum;

	if (unlikely(!toml || !*toml || !field))
		return (error_invalid_arg);
	field_buf = NULL;
	if (args)
	{
		field_buf = _toml_format_field(field, args);
		if (unlikely(!field_buf))
			return (error_alloc_fail);
		field = field_buf;
	}
	depth = _toml_count_field(field);
	target = _toml_get_field(*toml, field, -1);
	if (!target)
	{
		mem_free(field_buf);
		return (error_none);
	}
	parent = depth <= 1 ? *toml : _toml_get_field(*toml, field, depth - 1);
	errnum = _toml_detach_child(parent, target, should_free);
	mem_free(field_buf);
	return (errnum);
}

char	*_toml_itoa(
	long long value
)
{
	t_toml_str	result;

	memset(&result, 0, sizeof(result));
	if (unlikely(_toml_append_int(&result, value) != error_none))
	{
		mem_free(result.content);
		return (NULL);
	}
	return (result.content);
}

char	*_toml_uitoa(
	unsigned long long value
)
{
	t_toml_str	result;

	memset(&result, 0, sizeof(result));
	if (unlikely(_toml_append_uint_base(&result, value, 10) != error_none))
	{
		mem_free(result.content);
		return (NULL);
	}
	return (result.content);
}

static char	*_toml_trim(char *str)
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

static void	_toml_strip_comment(char *line)
{
	int		in_quote;
	char	quote;
	size_t	i;

	in_quote = 0;
	quote = '\0';
	i = 0;
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

static char	*_toml_dup_unquoted(const char *value)
{
	t_toml_str	result;
	size_t		i;
	char		quote;

	memset(&result, 0, sizeof(result));
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

static int	_toml_value_type(const char *value)
{
	size_t	i;
	int		has_dot;
	int		has_digit;

	value = _toml_trim((char *)value);
	if (*value == '"' || *value == '\'')
		return (toml_tok_str);
	if (*value == '[')
		return (toml_tok_array);
	if (!strcmp(value, "true") || !strcmp(value, "false"))
		return (toml_tok_bool);
	has_dot = 0;
	has_digit = 0;
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
	char	*data;
	int		type;
	int		errnum;

	type = _toml_value_type(value);
	data = NULL;
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
	char	*end;
	char	*item_start;
	size_t	i;
	int		in_quote;
	char	quote;
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
	*end = '\0';
	start++;
	item_start = start;
	in_quote = 0;
	quote = '\0';
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
			current_table = mem_dup(_toml_trim(trimmed + 1),
					strlen(_toml_trim(trimmed + 1)) + 1);
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
