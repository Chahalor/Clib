/**
 * @file _edit.c
 * @brief Internal TOML tree mutation helpers.
*/
/* ----| Headers    |----- */
	/* Standard */
#include <string.h>

	/* Internal */
#include "_toml.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static int	_toml_replace_node_value(
	TOML *const target,
	const char *const data,
	const int type
)
{
	char	*new_data = NULL;

	if (unlikely(!target))
		return (error_invalid_arg);

	if (data)
	{
		new_data = setting->dup(data, strlen(data) + 1);
		if (unlikely(!new_data))
			return (error_alloc_fail);
	}

	if (type != toml_tok_array && type != toml_tok_table)
	{
		_toml_free_content(target->child);
		target->child = NULL;
	}

	setting->free(target->data);
	target->data = new_data;
	target->type = type;
	return (error_none);
}

static char	*_toml_format_field(
	const char *const field,
	va_list *const args
)
{
	t_toml_str	result = {0};

	if (unlikely(_toml_fill_format(field, &result, args) != error_none))
	{
		setting->free(result.content);
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
	TOML	*prev = NULL;

	if (unlikely(!parent || !target))
		return (error_invalid_arg);

	node = parent->child;
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

/* ----| Public     |----- */

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

	if (unlikely(!*toml))
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
				setting->free(split);
				return (error_alloc_fail);
			}

			errnum = _toml_add_child(current, child);
			if (unlikely(errnum != error_none))
			{
				_toml_free_content(child);
				setting->free(split);
				return (errnum);
			}
		}
		else if (i + 1 < len && child->type != toml_tok_table)
		{
			setting->free(child->data);
			child->data = NULL;
			_toml_free_content(child->child);
			child->child = NULL;
			child->type = toml_tok_table;
		}

		current = child;
		i++;
	}
	setting->free(split);
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

	if (unlikely(!toml || !field || (value && value->type != type)))
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
		setting->free(field_buf);
		return (error_none);
	}

	parent = depth <= 1 ? *toml : _toml_get_field(*toml, field, depth - 1);
	errnum = _toml_detach_child(parent, target, should_free);
	setting->free(field_buf);
	return (errnum);
}

int	_toml_set_number_va_args(
	TOML *const			toml,
	const char *const	field,
	const char *const	data,
	va_list *const		args
)
{
	t_toml_str	str_field = {0};
	TOML		*root;
	int			errnum;

	root = toml;
	errnum = _toml_fill_format(field, &str_field, args);
	if (unlikely(errnum != error_none))
		goto cleanup;

	errnum = _toml_set_field(&root, str_field.content, data, toml_tok_int);

cleanup:
	setting->free(str_field.content);
	return (errnum);
}

int	_toml_set_container_va(
	TOML *const			toml,
	const char *const	field,
	TOML *const			var,
	const int			type,
	va_list *const		args
)
{
	t_toml_str	str_field = {0};
	TOML		*root;
	int			errnum;

	root = toml;
	errnum = _toml_fill_format(field, &str_field, args);
	if (unlikely(errnum != error_none))
		goto cleanup;

	errnum = _toml_set_node(&root, str_field.content, var, type);

cleanup:
	setting->free(str_field.content);
	return (errnum);
}

int	_toml_set_wild(
	TOML *const toml,
	const char *const field,
	void *var,
	int type,
	va_list *const args
)
{
	t_toml_str	str_field = {0};
	TOML		*root;
	int			errnum;
	char		*data;

	root = toml;
	data = NULL;
	errnum = _toml_fill_format(field, &str_field, &args);
	if (unlikely(errnum != error_none))
		goto cleanup;
	if (type == toml_tok_table || type == toml_tok_array)
		errnum = _toml_set_node(&root, str_field.content, var, type);
	else if (type == toml_tok_str || type == toml_tok_bool
		|| type == toml_tok_int || type == toml_tok_float
		|| type == toml_tok_datetime || type == toml_tok_null)
	{
		if (type == toml_tok_null)
			errnum = _toml_set_field(&root, str_field.content, NULL, type);
		else if (!var)
			errnum = error_invalid_arg;
		else
		{
			if (type == toml_tok_int)
				data = _toml_itoa(*(const int *)var);
			else if (type == toml_tok_bool)
				data = setting->dup(*(const int *)var ? "true" : "false",
						*(const int *)var ? sizeof("true") : sizeof("false"));
			else
				data = setting->dup(var, strlen((const char *)var) + 1);
			if (unlikely(!data))
				errnum = error_alloc_fail;
			else
				errnum = _toml_set_field(&root, str_field.content, data, type);
		}
	}
	else
		errnum = error_invalid_arg;

cleanup:
	setting->free(data);
	setting->free(str_field.content);
	return (errnum);
}