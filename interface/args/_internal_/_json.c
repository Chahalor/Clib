/**
 * @file _json.c
 * @brief Build an args parser definition from a JSON object.
 */

#include <string.h>

#include "_args.h"

#ifdef INTERFACE_ARGS_USE_JSON

static t_json	*_get_field(
	JSON *const json,
	const char *const name,
	const char *const alias
)
{
	t_json	*node;

	node = _json_get_field(json, name, -1);
	if (!node && alias)
		node = _json_get_field(json, alias, -1);
	return (node);
}

static const char	*_node_string(const t_json *const node)
{
	if (!node || node->type != json_tok_str)
		return (NULL);
	return (node->data);
}

static t_json	*_get_desc_field(JSON *const json)
{
	t_json	*node;

	node = _get_field(json, "desc", "description");
	if (!node)
		node = _get_field(json, "dsc", NULL);
	return (node);
}

static JSON	*_get_array(
	JSON *const json,
	const char *const name,
	const char *const alias,
	int *const error
)
{
	t_json	*node;

	node = _get_field(json, name, alias);
	if (!node)
		return (NULL);
	if (node->type != json_tok_array)
	{
		*error = -1;
		return (NULL);
	}
	return ((JSON *)node);
}

static int	_str_to_spec(const char *const str)
{
	if (!str)
		return (-1);
	if (!strcmp(str, "require") || !strcmp(str, "required")
		|| !strcmp(str, "requiered"))
		return (args_param_specs_require);
	if (!strcmp(str, "nargs"))
		return (args_param_specs_nargs);
	return (-1);
}

static int	_str_to_type(
	const char *const str,
	t_param_type *const type
)
{
	if (!str || !strcmp(str, "str") || !strcmp(str, "string"))
		*type = param_type_str;
	else if (!strcmp(str, "file"))
		*type = param_type_file;
	else if (!strcmp(str, "int") || !strcmp(str, "number"))
		*type = param_type_int;
	else if (!strcmp(str, "uint") || !strcmp(str, "unumber"))
		*type = param_type_uint;
	else
		return (-1);
	return (0);
}

static int	_json_to_specs(
	JSON *const array,
	int *const specs
)
{
	int	value;

	*specs = 0;
	if (!array)
		return (0);
	if (!json_assert_type(array, json_tok_array))
		return (-1);
	json_foreach(spec, array)
	{
		if (spec->type != json_tok_str)
			return (-1);
		value = _str_to_spec(spec->data);
		if (value < 0)
			return (-1);
		*specs |= value;
	}
	return (0);
}

static int	_add_param(
	JSON *const node,
	void *const parent,
	const bool is_parser
)
{
	t_json			*name_node;
	t_json			*desc_node;
	t_json			*type_node;
	t_json			*specs_node;
	const char		*name;
	const char		*desc;
	t_param_type	type;
	int				specs;

	if (!json_assert_type(node, json_tok_obj))
		return (-1);
	name_node = _get_field(node, "name", NULL);
	desc_node = _get_desc_field(node);
	type_node = _get_field(node, "type", NULL);
	specs_node = _get_field(node, "specs", NULL);
	name = _node_string(name_node);
	desc = _node_string(desc_node);
	if (!name || !name[0]
		|| (desc_node && !desc)
		|| (type_node && type_node->type != json_tok_str)
		|| (specs_node && specs_node->type != json_tok_array)
		|| _str_to_type(type_node ? type_node->data : NULL, &type)
		|| _json_to_specs((JSON *)specs_node, &specs))
		return (-1);
	if (is_parser)
		return (_args_parser_add_param(parent, name, desc, specs, type)
			? 0 : -1);
	return (_args_option_add_param(parent, name, desc, specs, type)
		? 0 : -1);
}

static int	_set_params(
	void *const object,
	JSON *const params,
	const bool is_parser
)
{
	json_foreach(param, params)
	{
		if (_add_param(param, object, is_parser))
			return (-1);
	}
	return (0);
}

static int	_add_option(
	JSON *const node,
	t_args_parser *const parser
)
{
	t_json			*short_node;
	t_json			*long_node;
	t_json			*desc_node;
	const char		*short_name;
	const char		*long_name;
	const char		*desc;
	t_args_option	*option;
	JSON			*params;
	int				error;

	if (!json_assert_type(node, json_tok_obj))
		return (-1);
	short_node = _get_field(node, "short-name", "short");
	long_node = _get_field(node, "long-name", "long");
	desc_node = _get_desc_field(node);
	short_name = _node_string(short_node);
	long_name = _node_string(long_node);
	desc = _node_string(desc_node);
	if ((short_node && !short_name) || (long_node && !long_name)
		|| (desc_node && !desc)
		|| ((!long_name || !long_name[0]) && (!short_name || !short_name[0]))
		|| (short_name && (!short_name[0] || short_name[1])))
		return (-1);
	option = _args_parser_add_option(
		parser, long_name, short_name ? short_name[0] : 0, desc);
	if (!option)
		return (-1);
	error = 0;
	params = _get_array(node, "params", "parameters", &error);
	if (error || (params && _set_params(option, params, false)))
		return (-1);
	return (0);
}

static int	_set_options(
	t_args_parser *const parser,
	JSON *const json
)
{
	JSON	*options;
	int		error;

	error = 0;
	options = _get_array(json, "opts", "options", &error);
	if (error)
		return (-1);
	json_foreach(option, options)
	{
		if (_add_option(option, parser))
			return (-1);
	}
	return (0);
}

static int	_set_subparsers(
	t_args_parser *const parser,
	JSON *const json
)
{
	JSON	*subparsers;
	int		error;

	error = 0;
	subparsers = _get_array(json, "sub-parser", "sub", &error);
	if (error)
		return (-1);
	json_foreach(sub, subparsers)
	{
		t_args_parser	*child;

		child = _args_parser_json(sub);
		if (!child)
			return (-1);
		_args_add_sub(parser, child);
	}
	return (0);
}

t_args_parser	*_args_parser_json(JSON *const json)
{
	t_json			*name_node;
	t_json			*desc_node;
	const char		*name;
	const char		*desc;
	t_args_parser	*parser;
	JSON			*params;
	int				error;

	if (!json_assert_type(json, json_tok_obj))
		return (NULL);
	name_node = _get_field(json, "name", NULL);
	desc_node = _get_desc_field(json);
	name = _node_string(name_node);
	desc = _node_string(desc_node);
	if ((name_node && !name) || (desc_node && !desc))
		return (NULL);
	parser = _args_mem_new_parser(name, desc);
	if (!parser)
		return (NULL);
	error = 0;
	params = _get_array(json, "params", "parameters", &error);
	if (error || (params && _set_params(parser, params, true))
		|| _set_options(parser, json) || _set_subparsers(parser, json))
	{
		_args_mem_free_parser(parser, true);
		return (NULL);
	}
	return (parser);
}

#endif
