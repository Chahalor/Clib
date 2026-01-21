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

ARGS_PARSER		*args_parser_new(void);

ARGS_SUB_PARSER	*args_parser_add_subparser(
					ARGS_PARSER *const parent,
					const char *const name
				);

ARGS_PARAM		*args_parser_add_param(
					ARGS_PARSER *const parser,
					const char *const name,
					const t_param_type type,
					const t_param_args_type spec
				);

ARGS_OPT		*args_parser_add_option(
					ARGS_PARSER *const parser,
					const char *const long_name,
					const char short_name
				);

ARGS_PARAM		*args_option_add_param(
					ARGS_OPT *const option,
					const char *const name,
					const t_param_type type,
					const t_param_args_type spec
				);

int				args_param_set_desc(
					ARGS_PARAM *const param,
					const char *const fmt,
					...
				);

int				args_option_set_desc(
					ARGS_OPT *const option,
					const char *const fmt,
					...
				);

int				args_parser_set_desc(
					ARGS_PARSER *const parser,
					const char *const fmt,
					...
				);

/*
 TODO: see to link the config to one parser
void			args_parser_set_config(
					ARGS_PARSER *parser,
					const t_args_config *config
				);

typedef int		(*t_args_param_check)(
					const t_args_output_param *param,
					void *user_data
				);

void			args_config_set_param_check(
					t_args_config *config,
					t_args_param_check fn,
					void *user_data
				);
*/

int				args_parse(
					const ARGS_PARSER *const parser,
					t_args_output *const output,
					const int argc,
					const char *argv[]
				);
