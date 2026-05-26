/**
 * 
*/
/* ----| Headers    |----- */
	/* Standard */
#include <string.h>
#include <unistd.h>

	/* Internal */
#include "toml.h"
#include "setting.h"
#include "_internal_/_toml.h"


	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

t_toml_settings	g_toml_settings = {
	.alloc = mem_alloc,
	.realloc = mem_realloc,
	.free = mem_free,
	.dup = mem_dup
};

TOML	*toml_new(void)
{
	return (_toml_new_content(NULL, toml_tok_table, NULL));
}

TOML	*toml_load_file(
	const char *const restrict path
)
{
	if (unlikely(!path))
		return (NULL);
	else
		return (_toml_load_file(path));
}

TOML	*toml_load_str(
	const char *const restrict format,
	...
)
{
	va_list	args;
	TOML	*result;

	if (unlikely(!format))
		return (NULL);

	va_start(args, format);
	result = _toml_load_str(format, &args);
	va_end(args);
	return (result);
}

int	toml_unload(
	TOML *toml
)
{
	if (unlikely(!toml))
		return (-error_invalid_arg);

	_toml_free_content(toml);
	return (error_none);
}

void	toml_free(
	TOML *const restrict obj
)
{
	if (likely(obj))
		_toml_free_content(obj);
}

void	*toml_get(
	TOML *const obj,
	const char *const restrict key,
	...
)
{
	va_list	args;
	void	*result;

	if (unlikely(!obj || !key))
		return (NULL);

	va_start(args, key);
	result = _toml_get(obj, key);
	va_end(args);
	return (result);
}

size_t	toml_sizeof(
	const TOML *const restrict toml
)
{
	if (unlikely(!toml))
		return (0);

	return (_toml_node_size(toml));
}

size_t	toml_len(
	const TOML *const restrict toml
)
{
	size_t		count;
	const TOML	*node;

	if (unlikely(!toml || (toml->type != toml_tok_array && toml->type != toml_tok_table)))
		return (0);

	count = 0;
	node = toml->child;
	while (node)
	{
		count++;
		node = node->next;
	}

	return (count);
}

int	toml_set_signed_nb(
	TOML *const toml,
	const char *const field,
	signed long long var,
	...
)
{
	va_list	args;
	char	*data;
	int		errnum;

	if (unlikely(!toml || !field))
		return (error_invalid_arg);

	data = _toml_itoa(var);
	if (unlikely(!data))
		return (error_alloc_fail);

	va_start(args, var);
	errnum = _toml_set_number_va_args(toml, field, data, &args);
	va_end(args);
	setting->free(data);
	return (errnum);
}

int	toml_set_unsigned_nb(
	TOML *const toml,
	const char *const field,
	unsigned long long var,
	...
)
{
	va_list	args;
	char	*data;
	int		errnum;

	if (unlikely(!toml || !field))
		return (error_invalid_arg);

	data = _toml_uitoa(var);
	if (unlikely(!data))
		return (error_alloc_fail);

	va_start(args, var);
	errnum = _toml_set_number_va_args(toml, field, data, &args);
	va_end(args);
	setting->free(data);
	return (errnum);
}

int	toml_set_string(
	TOML *const toml,
	const char *const field,
	char *var,
	...
)
{
	va_list	args;
	int		result = error_none;

	if (unlikely(!toml || !field || !var))
		return (error_invalid_arg);

	va_start(args, var);
	result = _toml_set_string(toml, field, var, &args);
	va_end(args);
	return (result);
}

int	toml_set_obj(
	TOML *const toml,
	const char *const field,
	TOML *var,
	...
)
{
	va_list	args;
	int		errnum;

	if (unlikely(!toml || !field))
		return (error_invalid_arg);

	va_start(args, var);
	errnum = _toml_set_container_va(toml, field, var, toml_tok_table, &args);
	va_end(args);
	return (errnum);
}

int	toml_set_array(
	TOML *const toml,
	const char *const field,
	void *var,
	...
)
{
	va_list	args;
	int		errnum;

	if (unlikely(!toml || !field))
		return (error_invalid_arg);

	va_start(args, var);
	errnum = _toml_set_container_va(toml, field, var, toml_tok_array, &args);
	va_end(args);
	return (errnum);
}

int	toml_set_wild(
	TOML *const toml,
	const char *const field,
	void *var,
	int type,
	...
)
{
	va_list	args;
	int		result;

	if (unlikely(!toml || !field || !var))
		return (error_invalid_arg);

	va_start(args, var);
	result = _toml_set_wild(toml, field, var, &args);
	va_end(args);
	return (result);
}

static int	_toml_append_clone_child(
	TOML *const array,
	TOML *const src,
	const int type
)
{
	TOML	*node;

	if (unlikely(!src || src->type != type))
		return (error_invalid_arg);

	node = _toml_new_content(NULL, type, NULL);
	if (unlikely(!node))
		return (error_alloc_fail);
	if (src->child)
	{
		node->child = _toml_clone_node(src->child);
		if (unlikely(!node->child))
		{
			_toml_free_content(node);
			return (error_alloc_fail);
		}
	}

	return (_toml_add_child(array, node));
}

int	toml_array_append(
	TOML *const toml,
	void *const value,
	const int type
)
{
	TOML	*node;
	char	*data;

	if (unlikely(!toml || toml->type != toml_tok_array))
		return (error_invalid_arg);
	if (type == toml_tok_array || type == toml_tok_table)
		return (_toml_append_clone_child(toml, value, type));
	data = NULL;
	if (type == toml_tok_null)
		;
	else if (!value)
		return (error_invalid_arg);
	else if (type == toml_tok_int)
		data = _toml_itoa(*(const int *)value);
	else if (type == toml_tok_bool)
		data = setting->dup(*(const int *)value ? "true" : "false",
				*(const int *)value ? sizeof("true") : sizeof("false"));
	else if (type == toml_tok_str || type == toml_tok_float
		|| type == toml_tok_datetime)
		data = setting->dup(value, strlen((const char *)value) + 1);
	else
		return (error_invalid_arg);
	if (type != toml_tok_null && unlikely(!data))
		return (error_alloc_fail);
	node = _toml_new_content(NULL, type, data);
	if (unlikely(!node))
	{
		setting->free(data);
		return (error_alloc_fail);
	}
	return (_toml_add_child(toml, node));
}

int	toml_array_pop(
	TOML *const toml,
	const size_t index
)
{
	TOML	*node;
	TOML	*prev;
	size_t	i;

	if (unlikely(!toml || toml->type != toml_tok_array))
		return (error_invalid_arg);
	node = toml->child;
	prev = NULL;
	i = 0;
	while (node && i < index)
	{
		prev = node;
		node = node->next;
		i++;
	}
	if (!node)
		return (error_invalid_arg);
	if (prev)
		prev->next = node->next;
	else
		toml->child = node->next;
	node->next = NULL;
	_toml_free_content(node);
	return (error_none);
}

int	toml_set_from_array(
	TOML *toml,
	const char *const restrict field,
	void *const array,
	const size_t length,
	const int type,
	...
)
{
	va_list		args;
	t_toml_str	str_field;
	TOML		*array_node;
	size_t		i;
	int			errnum;

	if (unlikely(!toml || !field || (!array && type != toml_tok_null
				&& length > 0)))
		return (error_invalid_arg);
	memset(&str_field, 0, sizeof(str_field));
	array_node = _toml_new_content(NULL, toml_tok_array, NULL);
	if (unlikely(!array_node))
		return (error_alloc_fail);
	va_start(args, type);
	errnum = _toml_fill_format(field, &str_field, &args);
	va_end(args);
	if (unlikely(errnum != error_none))
		goto cleanup;
	i = 0;
	while (i < length)
	{
		if (type == toml_tok_str || type == toml_tok_float
			|| type == toml_tok_datetime)
			errnum = toml_array_append(array_node,
					((char **)array)[i], type);
		else if (type == toml_tok_int || type == toml_tok_bool)
			errnum = toml_array_append(array_node,
					&((int *)array)[i], type);
		else if (type == toml_tok_table || type == toml_tok_array)
			errnum = toml_array_append(array_node,
					((TOML **)array)[i], type);
		else if (type == toml_tok_null)
			errnum = toml_array_append(array_node, NULL, type);
		else
			errnum = error_invalid_arg;
		if (unlikely(errnum != error_none))
			goto cleanup;
		i++;
	}
	errnum = _toml_set_node(&toml, str_field.content, array_node, toml_tok_array);

cleanup:
	setting->free(str_field.content);
	_toml_free_content(array_node);
	return (errnum);
}

int	toml_unset(
	TOML *toml,
	const char *const restrict field,
	...
)
{
	va_list	args;
	int		errnum;

	if (unlikely(!toml || !field))
		return (error_invalid_arg);
	va_start(args, field);
	errnum = _toml_unset(&toml, field, 0, &args);
	va_end(args);
	return (errnum);
}

int	toml_remove(
	TOML *toml,
	const char *const restrict field,
	...
)
{
	va_list	args;
	int		errnum;

	if (unlikely(!toml || !field))
		return (error_invalid_arg);
	va_start(args, field);
	errnum = _toml_unset(&toml, field, 1, &args);
	va_end(args);
	return (errnum);
}

static int	_toml_append_indent(
	t_toml_str *const out,
	const int depth,
	const int pretty
)
{
	int	i;
	int	errnum;

	if (pretty <= 0)
		return (error_none);
	i = 0;
	while (i < depth * pretty)
	{
		errnum = _toml_str_append_char(out, ' ');
		if (unlikely(errnum != error_none))
			return (errnum);
		i++;
	}
	return (error_none);
}

static int	_toml_append_quoted(
	t_toml_str *const out,
	const char *const value
)
{
	size_t	i;
	int		errnum;

	errnum = _toml_str_append_char(out, '"');
	if (unlikely(errnum != error_none))
		return (errnum);
	i = 0;
	while (value && value[i])
	{
		if (value[i] == '"' || value[i] == '\\')
			errnum = _toml_str_append_char(out, '\\');
		else if (value[i] == '\n')
			errnum = _toml_str_append_n(out, "\\n", 2);
		else if (value[i] == '\t')
			errnum = _toml_str_append_n(out, "\\t", 2);
		else
			errnum = _toml_str_append_char(out, value[i]);
		if (unlikely(errnum != error_none))
			return (errnum);
		if (value[i] == '\n' || value[i] == '\t')
		{
			i++;
			continue ;
		}
		i++;
	}
	return (_toml_str_append_char(out, '"'));
}

static int	_toml_append_value(
	t_toml_str *const out,
	TOML *const node,
	const int pretty
)
{
	TOML	*child;
	int		errnum;

	if (node->type == toml_tok_str)
		return (_toml_append_quoted(out, node->data));
	if (node->type == toml_tok_array)
	{
		errnum = _toml_str_append_char(out, '[');
		if (unlikely(errnum != error_none))
			return (errnum);
		child = node->child;
		while (child)
		{
			errnum = _toml_append_value(out, child, pretty);
			if (unlikely(errnum != error_none))
				return (errnum);
			if (child->next)
			{
				errnum = _toml_str_append_n(out, pretty ? ", " : ",", pretty ? 2 : 1);
				if (unlikely(errnum != error_none))
					return (errnum);
			}
			child = child->next;
		}
		return (_toml_str_append_char(out, ']'));
	}
	if (node->type == toml_tok_null)
		return (_toml_str_append_n(out, "\"\"", 2));
	return (_toml_str_append_n(out, node->data ? node->data : "",
			strlen(node->data ? node->data : "")));
}

static int	_toml_append_key_value(
	t_toml_str *const out,
	TOML *const node,
	const int pretty
)
{
	int	errnum;

	errnum = _toml_append_indent(out, 0, 0);
	if (unlikely(errnum != error_none))
		return (errnum);
	errnum = _toml_str_append_n(out, node->key, strlen(node->key));
	if (unlikely(errnum != error_none))
		return (errnum);
	errnum = _toml_str_append_n(out, pretty ? " = " : "=", pretty ? 3 : 1);
	if (unlikely(errnum != error_none))
		return (errnum);
	errnum = _toml_append_value(out, node, pretty);
	if (unlikely(errnum != error_none))
		return (errnum);
	return (_toml_str_append_char(out, '\n'));
}

static char	*_toml_child_path(
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

static int	_toml_append_table(
	t_toml_str *const out,
	TOML *const table,
	const char *const prefix,
	const int pretty
)
{
	TOML	*node;
	char	*path;
	int		errnum;

	if (prefix && *prefix)
	{
		errnum = _toml_str_append_char(out, '[');
		if (unlikely(errnum != error_none))
			return (errnum);
		errnum = _toml_str_append_n(out, prefix, strlen(prefix));
		if (unlikely(errnum != error_none))
			return (errnum);
		errnum = _toml_str_append_n(out, "]\n", 2);
		if (unlikely(errnum != error_none))
			return (errnum);
	}
	node = table->child;
	while (node)
	{
		if (node->type != toml_tok_table)
		{
			errnum = _toml_append_key_value(out, node, pretty);
			if (unlikely(errnum != error_none))
				return (errnum);
		}
		node = node->next;
	}
	node = table->child;
	while (node)
	{
		if (node->type == toml_tok_table)
		{
			if (out->len && out->content[out->len - 1] != '\n')
				_toml_str_append_char(out, '\n');
			if (out->len && out->content[out->len - 1] == '\n'
				&& (!prefix || !*prefix))
				;
			path = _toml_child_path(prefix, node->key);
			if (unlikely(!path))
				return (error_alloc_fail);
			errnum = _toml_str_append_char(out, '\n');
			if (unlikely(errnum == error_none))
				errnum = _toml_append_table(out, node, path, pretty);
			setting->free(path);
			if (unlikely(errnum != error_none))
				return (errnum);
		}
		node = node->next;
	}
	return (error_none);
}

char	*toml_stringify(
	TOML *toml,
	const int pretty
)
{
	t_toml_str	out;

	if (unlikely(!toml))
		return (NULL);
	memset(&out, 0, sizeof(out));
	if (toml->type == toml_tok_table)
	{
		if (unlikely(_toml_append_table(&out, toml, NULL, pretty) != error_none))
		{
			setting->free(out.content);
			return (NULL);
		}
	}
	else if (unlikely(_toml_append_value(&out, toml, pretty) != error_none))
	{
		setting->free(out.content);
		return (NULL);
	}
	return (out.content);
}

int	toml_dump(
	TOML *const restrict toml,
	FILE *const file,
	const int pretty
)
{
	char	*str;
	int		fd;

	if (unlikely(!toml || !file))
		return (error_invalid_arg);
	fd = fileno(file);
	if (unlikely(fd < 0))
		return (error_invalid_file);
	str = toml_stringify(toml, pretty);
	if (unlikely(!str))
		return (error_alloc_fail);
	write(fd, str, strlen(str));
	setting->free(str);
	return (error_none);
}

int	toml_assert_type(
	const TOML *const node,
	const int type
)
{
	return (node ? node->type == type : 0);
}

int	toml_get_type(
	const TOML *const node
)
{
	return (node ? node->type : 0);
}
