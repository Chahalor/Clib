// Header

/*
 * _parser.c
 *
 * Single-file JSON parser into your structs.
 *
 * Prototype:
 *   int _json_parser_file(JSON *_json);
 *
 * Notes:
 *  - No comment support
 *  - Strings: no backslash escapes handled (stop at next '"')
 *  - null -> json_tok_null with data == NULL
 *  - bool -> json_tok_bool with data == strdup("0") or strdup("1")
 *  - numbers stored as strings in data
 *  - arrays: elements have key == NULL
 */

/* ----| Headers    |----- */
	/* Standard */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

	/* Internal */
#include "_json.h"
	/* External */
		//...

/* ----| Prototypes |----- */

static int	parse_value(
				t_json_parser *_p,
				t_json **_out,
				const char *_key
				);

/* ----| Internals  |----- */

#pragma region utils

static char	*dup_range(
	const char *_s,
	size_t _a,
	size_t _b
)
{
	size_t	_len = 0;
	char	*result;
	
	_len = (_b > _a) ?
		(_b - _a) :
		0;
	result = mem_alloc(_len + 1);
	if (unlikely(!result))
		return (NULL);
	memcpy(result, _s + _a, _len);
	result[_len] = '\0';
	return result;
}

static int	append_child(
	t_json **head,
	t_json *child
)
{
	t_json	*_cur;

	if (unlikely(!head || !child))
		return (json_error_arg);
	if (!*head)
	{
		*head = child;
		return (json_error_none);
	}
	_cur = *head;
	while (_cur->next)
		_cur = _cur->next;
	_cur->next = child;
	return (json_error_none);
}


void	skip_ws(
	t_json_parser *_p
)
{
	while (_p->i < _p->len && isspace((unsigned char)_p->buf[_p->i]))
		_p->i++;
}

#pragma region parser value

static char	*parse_string_raw(
	t_json_parser *_p
)
{
	size_t	_start = 0; 
	char	*result = NULL;

	_start = _p->i;
	while (_p->i < _p->len && _p->buf[_p->i] != '"')
		_p->i++;
	if (_p->i >= _p->len)
		return (NULL);
	result = dup_range(_p->buf, _start, _p->i);
	if (unlikely(!result))
		return (NULL);
	_p->i++;
	return (result);
}

static char	*parse_number_raw(
	t_json_parser *_p
)
{
	size_t	_start =0;
	
	_start = _p->i;
	if (_p->i < _p->len && (_p->buf[_p->i] == '-' || _p->buf[_p->i] == '+'))
		_p->i++;
	while (_p->i < _p->len && isdigit((unsigned char)_p->buf[_p->i]))
		_p->i++;
	if (_p->i < _p->len && _p->buf[_p->i] == '.')
	{
		_p->i++;
		while (_p->i < _p->len && isdigit((unsigned char)_p->buf[_p->i]))
			_p->i++;
	}
	/* exponent */
	if (_p->i < _p->len && (_p->buf[_p->i] == 'e' || _p->buf[_p->i] == 'E'))
	{
		_p->i++;
		if (_p->i < _p->len && (_p->buf[_p->i] == '+' || _p->buf[_p->i] == '-'))
			_p->i++;
		while (_p->i < _p->len && isdigit((unsigned char)_p->buf[_p->i]))
			_p->i++;
	}
	if (_p->i == _start)
		return (NULL);
	return (dup_range(_p->buf, _start, _p->i));
}

static int	parse_literal(
	t_json_parser *_p,
	t_json **_out,
	const char *_key
)
{
	t_json	*_n = NULL;
	char	*_d = NULL;

	if (_p->i + 4 <= _p->len && strncmp(_p->buf + _p->i, "null", 4) == 0)
	{
		_p->i += 4;
		_n = _json_new_content(_key, json_tok_null, NULL);
		if (unlikely(!_n))
			return (_error_alloc);
		*_out = _n;
		return (json_error_none);
	}
	if (_p->i + 4 <= _p->len && strncmp(_p->buf + _p->i, "true", 4) == 0)
	{
		_p->i += 4;
		_d = mem_dup("true", 5);
		if (unlikely(!_d))
			return (_error_alloc);
		_n = _json_new_content(_key, json_tok_bool, _d);
		if (unlikely(!_n))
		{
			mem_free(_d);
			return (_error_alloc);
		}
		*_out = _n;
		return (json_error_none);
	}
	if (_p->i + 5 <= _p->len && strncmp(_p->buf + _p->i, "false", 5) == 0)
	{
		_p->i += 5;
		_d = mem_dup("false", 6);
		if (unlikely(!_d))
			return (_error_alloc);
		_n = _json_new_content(_key, json_tok_bool, _d);
		if (unlikely(!_n))
		{
			mem_free(_d);
			return (_error_alloc);
		}
		*_out = _n;
		return (json_error_none);
	}
	return (json_error_parsing);
}

static int	parse_array(
	t_json_parser *_p,
	t_json **_out,
	const char *_key
)
{
	t_json	*_arr = NULL;
	t_json	*_elem = NULL;
	int		_r;
	
	_arr = _json_new_content(_key, json_tok_array, NULL);
	if (unlikely(!_arr))
		return (_error_alloc);
	skip_ws(_p);
	if (_p->i < _p->len && _p->buf[_p->i] == ']')
	{
		_p->i++;
		*_out = _arr;
		return (json_error_none);
	}
	while (1)
	{
		skip_ws(_p);
		_elem = NULL;
		_r = parse_value(_p, &_elem, NULL);
		if (unlikely(_r != json_error_none))
		{
			_json_free_content(_arr);
			return (_r);
		}
		else if (unlikely(append_child(&_arr->child, _elem) != json_error_none))
		{
			_json_free_content(_arr);
			return (_error_alloc);
		}

		skip_ws(_p);
		if (_p->i >= _p->len)
		{
			_json_free_content(_arr);
			return (json_error_parsing);
		}
		if (_p->buf[_p->i] == ',')
		{
			_p->i++;
			continue;
		}
		else if (_p->buf[_p->i] == ']')
		{
			_p->i++;
			break;
		}
		else
		{
			_json_free_content(_arr);
			return (json_error_parsing);
		}
	}
	*_out = _arr;
	return json_error_none;
}

int	parse_object(
	t_json_parser *_p,
	t_json **_out,
	const char *key
)
{
	t_json	*obj = NULL;
	t_json	*val = NULL;
	char	*k = NULL;
	int		_r;
	
	obj = _json_new_content(key, json_tok_obj, NULL);
	if (unlikely(!obj))
		return (_error_alloc);
	skip_ws(_p);
	if (_p->i < _p->len && _p->buf[_p->i] == '}')
	{
		_p->i++;
		*_out = obj;
		return (json_error_none);
	}
	while (1)
	{
		skip_ws(_p);
		if (_p->i >= _p->len || _p->buf[_p->i] != '"')
		{
			_json_free_content(obj);
			return (json_error_parsing);
		}
		_p->i++;
		k = parse_string_raw(_p);
		if (unlikely(!k))
		{
			_json_free_content(obj);
			return (json_error_parsing);
		}
		skip_ws(_p);
		if (_p->i >= _p->len || _p->buf[_p->i] != ':')
		{
			mem_free(k);
			_json_free_content(obj);
			return (json_error_parsing);
		}
		_p->i++;
		skip_ws(_p);
		val = NULL;
		_r = parse_value(_p, &val, k);
		mem_free(k);
		if (_r != json_error_none)
		{
			_json_free_content(obj);
			return (_r);
		}
		else if (append_child(&obj->child, val) != json_error_none)
		{
			_json_free_content(obj);
			return (_error_alloc);
		}

		skip_ws(_p);
		if (_p->i >= _p->len)
		{
			_json_free_content(obj);
			return (json_error_parsing);
		}
		if (_p->buf[_p->i] == ',')
		{
			_p->i++;
			continue;
		}
		else if (_p->buf[_p->i] == '}')
		{
			_p->i++;
			break;
		}
		else
		{
			_json_free_content(obj);
			return (json_error_parsing);
		}
	}
	*_out = obj;
	return (json_error_none);
}

static int	parse_value(
	t_json_parser *_p,
	t_json **_out,
	const char *key
)
{
	char	c;
	char	*s = NULL;
	t_json	*n = NULL;

	skip_ws(_p);
	if (_p->i >= _p->len)
		return (json_error_parsing);
	c = _p->buf[_p->i];

	if (c == '{')
	{
		_p->i++;
		return (parse_object(_p, _out, key));
	}
	if (c == '[')
	{
		_p->i++;
		return (parse_array(_p, _out, key));
	}
	if (c == '"')
	{
		_p->i++;
		s = parse_string_raw(_p);
		if (unlikely(!s))
			return (json_error_parsing);
		n = _json_new_content(key, json_tok_str, s);
		if (unlikely(!n))
			return (mem_free(s), _error_alloc);
		*_out = n;
		return (json_error_none);
	}
	if (c == '-' || c == '+' || isdigit((unsigned char)c))
	{
		s = parse_number_raw(_p);
		if (unlikely(!s))
			return (json_error_parsing);
		n = _json_new_content(key, json_tok_nbr, s);
		if (unlikely(!n))
			return (mem_free(s), _error_alloc);
		*_out = n;
		return (json_error_none);
	}
	else
		return (parse_literal(_p, _out, key));
}

#pragma region file management

char	*read_file_to_buffer(
	FILE *_f,
	size_t *_out_len
)
{
	long	_sz = 0;
	size_t	_got = 0;
	char	*buf = NULL;

	if (unlikely(!_f))
		return (NULL);
	else if (unlikely(fseek(_f, 0, SEEK_END) != 0))
		return (NULL);
	_sz = ftell(_f);
	if (unlikely(_sz < 0))
		return (NULL);
	rewind(_f);
	buf = mem_alloc(_sz + 1);
	if (unlikely(!buf))
		return (NULL);
	_got = fread(buf, 1, (size_t)_sz, _f);
	buf[_got] = '\0';
	if (_out_len)
		*_out_len = _got;
	return (buf);
}

/* ----| Public     |----- */

int	_json_parser_string(
	JSON *_json,
	char *_str,
	const size_t _len
)
{
	t_json_parser	_p;
	int			_r;
	t_json		*_root;

	_p = (t_json_parser){
		.buf = _str,
		.len = _len,
		.i = 0
	};
	skip_ws(&_p);
	_r = parse_value(&_p, &_root, NULL);
	if (unlikely(_r != json_error_none))
		return (_r);
	skip_ws(&_p);
	if (_p.i < _p.len)
	{
		_json_free_content(_root);
		return (json_error_parsing);
	}
	if (_json->content)
		_json_free_content(_json->content);
	_json->content = _root;
	_json->parsed = 1;
	return (json_error_none);
}

int	_json_parser_file(
	JSON *_json
)
{
	size_t	len = 0;
	char	*buf = NULL;
	int		_r = 0;

	buf = read_file_to_buffer(_json->file, &len);
	fclose(_json->file);
	_json->file = NULL;
	if (unlikely(!buf))
		return (json_error_io);
	_r = _json_parser_string(_json, buf, len);
	mem_free(buf);
	return (_r);
}

# pragma region debug

void	print_json_tree(
	t_json *node,
	int depth
)
{
	if (!node)
		return ;

	for (int i = 0; i < depth; ++i)
		printf("    ");

	if (node->key)
		printf("key: \"%s\": ", node->key);

	switch (node->type)
	{
		case json_tok_obj:
			printf("{(child)\n");
			print_json_tree(node->child, depth + 1);
			for (int i = 0; i < depth; ++i)
				printf("    ");
			printf("}\n");
			break;
		case json_tok_array:
			printf("[(child)\n");
			print_json_tree(node->child, depth + 1);
			for (int i = 0; i < depth; ++i)
				printf("    ");
			printf("]\n");
			break;
		case json_tok_str:
			printf("\"%s\"\n", node->data);
			break;
		case json_tok_nbr:
			printf("%s\n", node->data);
			break;
		case json_tok_bool:
			printf("%s\n", (size_t)node->data ? "true" : "false");
			break;
		case json_tok_null:
			printf("null\n");
			break;
		default:
			printf("(unknown type)\n");
			break;
	}
	print_json_tree(node->next, depth + 1);
}
