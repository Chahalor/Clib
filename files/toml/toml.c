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
	result = _toml_get(obj, key, &args);
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

	va_start(args, type);
	result = _toml_set_wild(toml, field, var, type, &args);
	va_end(args);
	return (result);
}

int	toml_array_append(
	TOML *const toml,
	void *const value,
	const int type
)
{
	if (unlikely(!toml || toml->type != toml_tok_array))
		return (error_invalid_arg);

	return (_toml_array_append(toml, value, type));
}

int	toml_array_pop(
	TOML *const		toml,
	const size_t	index
)
{
	if (unlikely(!toml || toml->type != toml_tok_array))
		return (error_invalid_arg);

	return (_toml_array_pop(toml, index));
}

int	toml_set_from_array(
	TOML *						toml,
	const char *const restrict	field,
	void *const					array,
	const size_t				length,
	const int					type,
	...
)
{
	va_list		args;
	int			result;

	if (unlikely(!toml || !field || (!array && type != toml_tok_null && length > 0)))
		return (error_invalid_arg);

	va_start(args, type);
	result = _toml_set_from_array(toml, field, array, length, type, &args);
	va_end(args);
	return (result);
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

char	*toml_stringify(
	TOML *toml,
	const int pretty
)
{
	if (unlikely(!toml))
		return (NULL);

	return (_toml_stringify(toml, pretty));
}

int	toml_dump(
	TOML *const restrict toml,
	FILE *const file,
	const int pretty
)
{
	if (unlikely(!toml || !file))
		return (error_invalid_arg);

	return (_toml_dump(toml, file, pretty));
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

int		toml_errno(void)
{
	return (_toml_error()->errnum);
}

const char	*toml_strerror(void)
{
	return (_toml_error_str(_toml_error()->errnum));
}

void		toml_perror(
	const char *const msg
)
{
	_toml_perror(msg);
}

void		toml_error_dump(
	FILE *const file
)
{
	_toml_error_dump(file);
}
