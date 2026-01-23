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

ARGSP		*args_parser_new(void);

// TODO: check for this
// #if __has_include ("json.h") || __has_include("files/json.h")

// ARGSP		*args_parser_load_json(
// 					JSON *const restrict json
// 				);

// #endif


// ARGSP		*args_parser_load_yaml(
// 					YAML *const restrict json
// 				);

void			args_destroy_parser(
					ARGSP *const restrict parser
				);

#pragma region Editing

ARGS_SUB_PARSER	*args_parser_add_subparser(
					ARGSP *const parent,
					const char *const name
				);

ARGS_PARAM		*args_parser_add_param(
					ARGSP *const parser,
					const char *const name,
					const t_param_type type,
					const t_param_args_type spec
				);

ARGS_OPT		*args_parser_add_option(
					ARGSP *const parser,
					const char *const long_name,
					const char short_name
				);

ARGS_PARAM		*args_option_add_param(
					ARGS_OPT *const option,
					const char *const name,
					const t_param_type type,
					const t_param_args_type spec
				);

/** */
int	args_set_desc(
	ARGSP *const target,
	const char *const fmt,
	...
);

/*
 TODO: see to link the config to one parser
void			args_parser_set_config(
					ARGSP *parser,
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

#pragma region Parsing

/** */
int				args_parse(
					ARGSP *const parser,
					const int argc,
					const char *argv[]
				);

#pragma region Extration

/**
 * @brief	return if a field exist in the parsed args. could be an option or param
 * 
 * @param	parser	the parser args
 * @param	field	the field name
 * 
 * @return bool, true or false
 * 
 * @example:
 * 	args_has(parser, "file") -> check for the option and param named file
 * 	args_has(parser, "--file") -> only check for an option named like that
 * 	args_has(parser, "-f") -> only check for an option named like that
*/
bool			args_has(
					const ARGSP *const restrict parser,
					const char *const restrict field
				);

/**
 * @brief	return if the option named exist in the parser and has been recvd
 * 
 * @param	parser	the parser args
 * @param	field	the field name
 * 
 * @return bool, true or false
 * 
 * @example:
 * 	args_has_opt(parser, "file") -> check for the option named file
 * 	args_has_opt(parser, "--file") -> check for the option named file
 * 	args_has_opt(parser, "-f") -> check for the option named file
*/
bool			args_has_opt(
					const ARGSP *const restrict parser,
					const char *const restrict field
				);

/**
 * @brief	return if the param named exist in the parser and has been recvd
 * 
 * @param	parser	the parser args
 * @param	field	the field name
 * 
 * @return bool: true or false
 * 
 * @example:
 * 	args_has_param(parser, "file") -> check for the param file is recved
*/
bool			args_has_param(
					const ARGSP *const restrict parser,
					const char *const restrict field
				);

/**
 * @brief	get the value of the option, param or sub-parser named `field`
 * in case of unrecvd or unregister param/option, the function will return the corresponding error
 * If the field is a sub-parser, return a pointer to the sub-parser. This pointer can be used 
 * 
 * @param	parser	the parser to extract data from
 * @param	field	the field of the extraction
 * @param	dest	the destination whre to put the values
 * 
 * @return	error_none in case of success or the error code
 * 
 * @example:
 * 	args_get(parser, "file", &filename) -> return `error_none` if "file" exist or the error code if not
 * 	args_get(parser, "--file", &filename) -> only search for the option named "file"
 * 	args_get(parser, "-f", &filename) -> only search for the option named with a short name 'f'
*/
int				args_get(
					const ARGSP *const restrict parser,
					const char *const restrict field,
					void *const restrict dest
				);

/**
 * @brief	return the value stored inside of the option named `field`
 * 
 * @param	parser	the pasrer
 * @param	field	the option name
 * @param	dest	the value destination
 * 
 * @return	error_none in case of success or the error code
 * 
 * @example:
 * 	args_get_opt(parser, "file", &filename) -> return the content of the option file  if any thing has been passed
 * 	args_get_opt(parser, "--file", &filename) -> same has the previous one
 * 	args_get_opt(parser, "-f", &filename) -> return the content of the option where the short_name == 'f'
*/
int				args_get_opt(
					const ARGSP *const restrict parser,
					const char *const restrict field,
					char **const dest
				);

/**
 * @brief	return the value stored inside of the param named `field`
 * 
 * @param	parser	the pasrer
 * @param	field	the option name
 * @param	dest	the value destination
 * 
 * @return	error_none in case of success or the error code
 * 
 * @example
 * 	args_get_opt(parser, "file", &filename) -> return the content of the param file if any thing has been passed
 * 	args_get_opt(parser, "--file", &filename) -> invalid, no param should be named like that
*/
int				args_get_param(
					const ARGSP *const restrict parser,
					const char *const restrict field,
					char **const dest
				);

/**
 * @brief	return the value stored inside of the param named `field`
 * 
 * @param	parser	the pasrer
 * @param	field	the option name
 * @param	dest	the value destination
 * 
 * @return	error_none in case of success or the error code
 * 
 * @example
 * 	args_get_opt(parser, "add", &sub_parser) -> return a pointer to the sub-parser `add`
 * 	args_get_opt(parser, "--add", &sub_parser) -> invalid, no sub-parser should be named like that
*/
int				args_get_sub_parser(
					const ARGSP *const restrict parser,
					const char *const restrict field,
					ARGS_SUB_PARSER **const dest
				);

#pragma region Config

/**
 * TODO: all function to edit the args config
 */