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

/** */
t_args_parser	*args_new_parser(void);

int				args_add_param(
					void *const restrict _parser,
					const char *const restrict _name,
					const t_param_type _type,
					const t_param_args_type _spec
					);

int				args_add_option(
					t_args_parser *const restrict _parser,
					const char *const restrict _name,
					const t_param_type _type,
					const t_param_args_type _args_spec
					);

int				args_add_subparser(
					t_args_parser *const restrict _parser,
					const char *const restrict _name
					);

int				args_param_add_desc(
					t_args_param **const restrict _param,
					const char *const restrict _format,
					...
					);

int				args_option_add_desc(
					t_args_option **const restrict _opt,
					const char *const restrict _format,
					...
					);

int				args_parser_add_desc(
					t_args_parser **const restrict _parser,
					const char *const restrict _format,
					...
					);

int				args_parse(
					const t_args_parser *const restrict _parser,
					t_args_output *const restrict _output,
					const int argc,
					const char *argv[]
					);

void			args_destroy_parser(
					t_args_parser *const restrict _parser
					);