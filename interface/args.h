// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "lib_config.h"
# include "lib_standards.h"

/* -----| Internals |----- */
# include "args/types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

#pragma region Memory

/** */
t_args_parser	*args_parser_new(void);

/** */
void			args_parser_free(
	t_args_parser *const _parser
);

void			args_output_free(
	t_args_output *const _out
);

#pragma region Parsing

/** */
t_args_output	*args_parse(
	const t_args_parser *const _parser,
	const int _argc,
	const char *const _argv[]
);

#pragma region Sub-parsers

/** */
t_args_parser	*args_parser_add_sub(
	t_args_parser *const _parent,
	const char *const _name,
	const char *const _desc
);

#pragma region Options

t_args_option	*args_parser_add_option(
	t_args_parser *const _parser,
	const char *const _long_name,
	const char _short_name,
	const char *const _desc
);

#pragma region Parameters

t_args_param	*args_parser_add_param(
	t_args_parser *const _parser,
	const char *const _name,
	const char *const _desc,
	const int _spec,
	const int _type
);

t_args_param	*args_option_add_param(
	t_args_option *const _parent,
	const char *const _name,
	const char *const _desc,
	const int _spec,
	const int _type
);

#pragma region Setter

int args_parser_set_desc(
	t_args_parser *const parser,
	const char *const desc
);

int	args_param_set_desc(
	t_args_param *const _param,
	const char *const _desc
);

int	args_option_set_desc(
	t_args_option *const _option,
	const char *const _desc
);

#pragma region Extraction

char	args_has_param(
	t_args_output *const _parser,
	const char *const _name
);

char	args_has_option(
	t_args_output *const _parser,
	const char *const _name
);

char	args_has_sub(
	t_args_output *const _parser,
	const char *const _name
);

char	args_option_has_param(
	t_args_output_option *const _option,
	const char *const _name
);

char	*args_get_param(
	t_args_output *const _output,
	const char *const _name,
	char *const * *const _values,
	unsigned int *const _count
);

char	*args_output_parser_get_param(
	t_args_output_parser *const _output,
	const char *const _name,
	char *const * *const _values,
	unsigned int *const _count
);

char	args_get_option(
	t_args_output *const _output,
	const char *const _name,
	char *const * *const _values,
	unsigned int *const _count
);

char	args_output_parser_get_option(
	t_args_output_parser *const _output,
	const char *const _name,
	char *const * *const _values,
	unsigned int *const _count
);

const char	*args_active_subcommand(
	const t_args_output *out
);

t_args_output_parser	*args_get_sub_output(
	const t_args_output *out
);


#pragma region Configuration

int	args_error(
	const t_args_output *out
);

const char	*args_error_str(
	int err
);
