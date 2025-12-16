// Header

/* ----| Headers    |----- */
	/* Standard */
#include <unistd.h>
#include <string.h>

	/* Internal */
#include "_json.h"
	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Macros     |----- */

#define NEWLINE_AND_INDENT(_depth) \
	do { \
		_json_string_add_pretty(_depth, _pretty, result); \
	} while (0)

		// _json_string_add(result, "\n", 0);

/* ----| Internals  |----- */

static int _json_string_add(
	t_json_str *result,
	const char *restrict _str,
	int _has_quote
)
{
	int		_add_len = 0;
	char	*_new = NULL;
	int		_new_size = 0;

	if ((!_str))
	{
		_str = "null";
		_has_quote = 0;
	}

	_add_len = strlen(_str) + (_has_quote ? 2 : 0);
	if (result->len + _add_len + 1 > result->size)
	{
		_new_size = result->size
						? result->size
						: INTERFACE_JSON_STRING_ALLOC_SIZE;

		while (result->len + _add_len + 1 > _new_size)
			_new_size *= 2;

		_new = mem_alloc(_new_size);
		if (unlikely(!_new))
			return (error_alloc_fail);
		memcpy(_new, result->content, result->size);
		mem_free(result->content);
		result->content = _new;
		result->size = _new_size;
	}

	if (_has_quote)
	{
		result->content[result->len++] = '"';
		memcpy(result->content + result->len, _str, strlen(_str));
		result->len += strlen(_str);
		result->content[result->len++] = '"';
	}
	else
	{
		memcpy(result->content + result->len, _str, strlen(_str));
		result->len += strlen(_str);
	}
	result->content[result->len] = '\0';
	return (error_none);
}


void	_json_string_add_pretty(
	const int _depth,
	const char *const restrict _pretty,
	t_json_str *const result
)
{
	if (!_pretty)
		return ;
	for (int i = 0; i < _depth; ++i)
		_json_string_add(result, (void *)_pretty, 0);
}

static void	_json_string_add_tree_no_pretty(
	t_json *_node,
	t_json_str *result
)
{
	if (!_node)
		return;

	if (_node->key)
	{
		_json_string_add(result, _node->key, 1);
		_json_string_add(result, ": ", 0);
	}

	switch (_node->type)
	{
		case json_tok_obj:
			_json_string_add(result, "{", 0);
			_json_string_add_tree_no_pretty(_node->child, result);
			_json_string_add(result, "}", 0);
			break;

		case json_tok_array:
			_json_string_add(result, "[", 0);
			_json_string_add_tree_no_pretty(_node->child, result);
			_json_string_add(result, "]", 0);
			break;

		case json_tok_str:
			_json_string_add(result, _node->data, 1);
			break;

		case json_tok_nbr:
			_json_string_add(result, _node->data, 0);
			break;

		case json_tok_bool:
			_json_string_add(result, ((size_t)_node->data) ? "true" : "false", 0);
			break;

		case json_tok_null:
			_json_string_add(result, "null", 0);
			break;
	}

	if (likely(_node->next))
	{
		_json_string_add(result, ",", 0);
		_json_string_add_tree_no_pretty(_node->next, result);
	}
}

static void	_json_string_add_tree_pretty(
	t_json *_node,
	int _depth,
	t_json_str *result,
	char *_pretty
)
{
	if (!_node)
		return;

	_json_string_add_pretty(_depth, _pretty, result);

	if (_node->key)
	{
		_json_string_add(result, _node->key, 1);
		_json_string_add(result, ": ", 0);
	}

	switch (_node->type)
	{
		case json_tok_obj:
			_json_string_add(result, "{\n", 0);
			_json_string_add_tree_pretty(_node->child, _depth + 1, result, _pretty);
			_json_string_add_pretty(_depth, _pretty, result);
			_json_string_add(result, "}", 0);
			break;

		case json_tok_array:
			_json_string_add(result, "[\n", 0);
			_json_string_add_tree_pretty(_node->child, _depth + 1, result, _pretty);
			_json_string_add_pretty(_depth, _pretty, result);
			_json_string_add(result, "]", 0);
			break;

		case json_tok_str:
			_json_string_add(result, _node->data, 1);
			break;
		
		case json_tok_nbr:
			_json_string_add(result, _node->data, 0);
			break;

		case json_tok_bool:
			_json_string_add(result, ((size_t)_node->data) ? "true" : "false", 0);
			break;

		case json_tok_null:
			_json_string_add(result, "null", 0);
			break;
	}

	if (_node->next)
	{
		_json_string_add(result, ",\n", 0);
		_json_string_add_tree_pretty(_node->next, _depth, result, _pretty);
	}
	else
		_json_string_add(result, "\n", 0);
}


void	_json_string_add_tree(
	t_json *_node,
	t_json_str *result,
	const char *_pretty
)
{

	return (_pretty
			? _json_string_add_tree_pretty(_node, 0, result, (void *)_pretty)
			: _json_string_add_tree_no_pretty(_node, (void *)result)
		);
}


/* ----| Public     |----- */

char	*_json_stringify(
	JSON *_json,
	const int pretty
)
{
	t_json_str	result = {0};
	char		*_pretty = NULL;

	result.content = mem_alloc(INTERFACE_JSON_STRING_ALLOC_SIZE);
	if (unlikely(!result.content))
		return (NULL);
	result.size = INTERFACE_JSON_STRING_ALLOC_SIZE;

	if (pretty)
	{
		_pretty = mem_alloc(sizeof(char) * (pretty + 1));
		if (likely(_pretty))
			memset(_pretty, ' ', pretty);
	}
	_json_string_add_tree(_json, &result, _pretty);
	return (result.content);
}

int		_json_dump(
	JSON *_json,
	FILE *_file,
	const int pretty
)
{
	char	*_str = NULL;
	int		_fd = -1;

	_fd = fileno(_file);
	if (unlikely(_fd < 0))
		return (json_error_io);
	_str = _json_stringify(_json, pretty);
	if (unlikely(!_str))
	{
		write(_fd, "{(null)}", 9);
		return (error_none);
	}
	write(_fd, _str, strlen(_str));
	mem_free(_str);
	return (error_none);
}
