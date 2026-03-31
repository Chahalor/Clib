/**
 * @file args.h
 * @brief Public API for the `interface/args` command-line parser module.
 *
 * This module lets you define a parser (options, positional parameters,
 * subcommands) and parse `argv` into an output tree.
 *
 * Supported features:
 * - Short options (`-v`) and long options (`--verbose`)
 * - Subcommands
 * - Positional parameters
 * - Option-bound parameters
 * - Multi-value parameters (`args_param_specs_nargs`)
 * - `--` delimiter to stop option detection
 * - builtin `-h/--help` option
 *
 * @defgroup args Args Parser
 * @brief Public interfaces for command-line parsing.
 * @{
 */

#ifndef INTERFACE_ARGS_H
# define INTERFACE_ARGS_H

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "../standards/lib_config.h"
# include "../standards/lib_standards.h"

/* -----| Internals |----- */
# include "args/types.h"

/* -----| Modules   |----- */
# include "../processor/processor.h"

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

#pragma region Memory

/**
 * @brief Create a new root parser.
 *
 * The root parser is initialized empty and includes the built-in
 * `help` option (`--help` / `-h`).
 *
 * @return New parser instance, or `NULL` on allocation failure.
 */
t_args_parser	*args_parser_new(void);

/**
 * @brief Free a parser definition tree.
 *
 * @param _parser Parser to free (`NULL` is allowed).
 */
void			args_parser_free(
	t_args_parser *const _parser
);

/**
 * @brief Free a parse output tree.
 *
 * @param _out Output to free (`NULL` is allowed).
 */
void			args_output_free(
	t_args_output *const _out
);

#pragma region Parsing

/**
 * @brief Parse command-line arguments using a parser definition.
 *
 * `argv[0]` is treated as program name and ignored by the parser logic.
 *
 * @param _parser Root parser definition.
 * @param _argc   Argument count.
 * @param _argv   Argument vector.
 *
 * @return Parse output (including parse errors when available), or `NULL`
 *         on allocation/argument failure.
 */
t_args_output	*args_parse(
	const t_args_parser *const _parser,
	const int _argc,
	const char *const _argv[]
);

/**
 * @brief Render help for a parser.
 *
 * @param _parser Parser to describe.
 * @param _exit   Exit behavior flag used by the module.
 *
 * @note Current implementation exits the process after printing help.
 */
void	args_show_help(
	const t_args_parser *const _parser,
	const int _exit
);

#pragma region Sub-parsers

/**
 * @brief Add a subcommand parser to a parser.
 *
 * @param _parent Parent parser.
 * @param _name   Subcommand name.
 * @param _desc   Optional description.
 *
 * @return New sub-parser, or `NULL` on error.
 */
t_args_parser	*args_parser_add_sub(
	t_args_parser *const _parent,
	const char *const _name,
	const char *const _desc
);

#pragma region Options

/**
 * @brief Add an option to a parser.
 *
 * @param _parser     Target parser.
 * @param _long_name  Long option name (with or without leading dashes).
 * @param _short_name Short option name (`0` if unused).
 * @param _desc       Optional description.
 *
 * @return New option, or `NULL` on error.
 */
t_args_option	*args_parser_add_option(
	t_args_parser *const _parser,
	const char *const _long_name,
	const char _short_name,
	const char *const _desc
);

#pragma region Parameters

/**
 * @brief Add a parameter to a parser or an option.
 *
 * @param var         Target node (`t_args_parser *` or `t_args_option *`).
 * @param name        Parameter name.
 * @param description Optional description.
 * @param spec        Combination of `t_args_param_specs`.
 * @param type        Logical parameter type (`t_param_type`).
 *
 * @return New parameter, or `NULL` on error.
 */
# define	args_add_param(var, name, description, spec, type) (\
	__builtin_choose_expr(\
		IS_TYPE(var, t_args_parser *),\
		args_parser_add_param,\
		__builtin_choose_expr(\
			IS_TYPE(var, t_args_option *),\
			args_option_add_param,\
			(void *)0\
		)\
	)\
	(var, name, description, spec, type)\
)

/**
 * @brief Add a positional parameter to a parser.
 *
 * @param _parser Target parser.
 * @param _name   Parameter name.
 * @param _desc   Optional description.
 * @param _spec   Combination of `t_args_param_specs`.
 * @param _type   Logical parameter type (`t_param_type`).
 *
 * @return New parameter, or `NULL` on error.
 */
t_args_param	*args_parser_add_param(
	t_args_parser *const _parser,
	const char *const _name,
	const char *const _desc,
	const int _spec,
	const int _type
);

/**
 * @brief Add an option-bound parameter to an option.
 *
 * @param _parent Target option.
 * @param _name   Parameter name.
 * @param _desc   Optional description.
 * @param _spec   Combination of `t_args_param_specs`.
 * @param _type   Logical parameter type (`t_param_type`).
 *
 * @return New parameter, or `NULL` on error.
 */
t_args_param	*args_option_add_param(
	t_args_option *const _parent,
	const char *const _name,
	const char *const _desc,
	const int _spec,
	const int _type
);

#pragma region Setter

/**
 * @brief Set description text on parser/parameter/option nodes.
 *
 * @param var         Target node (`t_args_parser *`, `t_args_param *`,
 *                    or `t_args_option *`).
 * @param description New description string (`NULL` to clear).
 *
 * @return `error_none` on success, otherwise an error code.
 */
# define	args_set_description(var, description) (__builtin_choose_expr( \
	IS_TYPE(var, t_args_parser *), \
	args_parser_set_desc, \
	__builtin_choose_expr( \
		IS_TYPE(var, t_args_param *), \
		args_param_set_desc, \
		__builtin_choose_expr( \
			IS_TYPE(var, t_args_option *), \
			args_option_set_desc, \
			(void *)0 \
		) \
	) \
)(var, description))

/**
 * @brief Set a parser description.
 *
 * @param parser Target parser.
 * @param desc   New description (`NULL` to clear).
 *
 * @return `error_none` on success, otherwise an error code.
 */
int args_parser_set_desc(
	t_args_parser *const parser,
	const char *const desc
);

/**
 * @brief Set a parameter description.
 *
 * @param _param Target parameter.
 * @param _desc  New description (`NULL` to clear).
 *
 * @return `error_none` on success, otherwise an error code.
 */
int	args_param_set_desc(
	t_args_param *const _param,
	const char *const _desc
);

/**
 * @brief Set an option description.
 *
 * @param _option Target option.
 * @param _desc   New description (`NULL` to clear).
 *
 * @return `error_none` on success, otherwise an error code.
 */
int	args_option_set_desc(
	t_args_option *const _option,
	const char *const _desc
);

#pragma region Extraction

/**
 * @brief Check whether a root positional parameter exists in output.
 *
 * @param _parser Parse output.
 * @param _name   Parameter name.
 *
 * @return `1` if present, `0` if absent, or `error_invalid_arg`.
 */
char	args_has_param(
	t_args_output *const _parser,
	const char *const _name
);

/**
 * @brief Check whether an option exists in output.
 *
 * Accepted names: `-x`, `--name`, `x`, `name`.
 *
 * @param _parser Parse output.
 * @param _name   Option name.
 *
 * @return `1` if present, `0` if absent, or `error_invalid_arg`.
 */
char	args_has_option(
	t_args_output *const _parser,
	const char *const _name
);

/**
 * @brief Check whether a subcommand exists in output.
 *
 * @param _parser Parse output.
 * @param _name   Subcommand name.
 *
 * @return `1` if present, `0` if absent, or `error_invalid_arg`.
 */
char	args_has_sub(
	t_args_output *const _parser,
	const char *const _name
);

/**
 * @brief Check whether an output option contains a named parameter.
 *
 * @param _option Output option.
 * @param _name   Parameter name.
 *
 * @return `1` if present, `0` if absent, or `error_invalid_arg`.
 */
char	args_option_has_param(
	t_args_output_option *const _option,
	const char *const _name
);

/**
 * @brief Get parameter value(s) from a root output or sub-output node.
 *
 * @param var  Source node (`t_args_output *` or `t_args_output_parser *`).
 * @param name Parameter name.
 * @param n    Receives number of values.
 *
 * @return `NULL` if not found or empty; `char *` when `*n == 1`; `char **`
 *         (NULL-terminated) when `*n > 1`. Returned memory must be freed
 *         with `mem_free`.
 */
# define	args_get_param(var, name, n) (__builtin_choose_expr( \
		IS_TYPE(var, t_args_output *), \
		args_parser_get_param, \
		__builtin_choose_expr( \
			IS_TYPE(var, t_args_output_parser *), \
			args_output_parser_get_param, \
			(void)0 \
		) \
	)(var, name, n))

/**
 * @brief Get an option node from a root output or sub-output node.
 *
 * @param var  Source node (`t_args_output *` or `t_args_output_parser *`).
 * @param name Option name.
 *
 * @return Matching output option, or `NULL` if not found.
 */
# define	args_get_option(var, name) (__builtin_choose_expr( \
		IS_TYPE(var, t_args_output *), \
		args_parser_get_option, \
		__builtin_choose_expr( \
			IS_TYPE(var, t_args_output_parser *), \
			args_output_parser_get_option, \
			(void)0 \
		) \
	)(var, name))

/**
 * @brief Get value(s) for a named root positional parameter.
 *
 * @param output Parse output root.
 * @param name   Parameter name.
 * @param n      Receives number of values.
 *
 * @return `NULL` if not found or empty; `char *` when `*n == 1`; `char **`
 *         (NULL-terminated) when `*n > 1`. Returned memory must be freed
 *         with `mem_free`.
 */
void	*args_parser_get_param(
	t_args_output *const output,
	const char *const name,
	size_t *const n
);

/**
 * @brief Get value(s) for a named subcommand positional parameter.
 *
 * @param parser Subcommand output node.
 * @param name   Parameter name.
 * @param n      Receives number of values.
 *
 * @return `NULL` if not found or empty; `char *` when `*n == 1`; `char **`
 *         (NULL-terminated) when `*n > 1`. Returned memory must be freed
 *         with `mem_free`.
 */
void	*args_output_parser_get_param(
	t_args_output_parser *const parser,
	const char *const name,
	size_t *const n
);

/**
 * @brief Get a named option from root output.
 *
 * @param output Parse output root.
 * @param name   Option name.
 *
 * @return Matching output option, or `NULL` if not found.
 */
t_args_output_option	*args_parser_get_option(
	t_args_output *const output,
	const char *const name
);

/**
 * @brief Get a named option from a subcommand output.
 *
 * @param parser Subcommand output node.
 * @param name   Option name.
 *
 * @return Matching output option, or `NULL` if not found.
 */
t_args_output_option	*args_output_parser_get_option(
	t_args_output_parser *const parser,
	const char *const name
);

/**
 * @brief Return the active subcommand name (first matched subcommand).
 *
 * @param out Parse output.
 *
 * @return Active subcommand name, or `NULL`.
 */
const char	*args_active_subcommand(
	const t_args_output *out
);

/**
 * @brief Return the first subcommand output node.
 *
 * @param out Parse output.
 *
 * @return Subcommand output node, or `NULL`.
 */
t_args_output_parser	*args_get_sub_output(
	const t_args_output *out
);

#pragma region Configuration

/**
 * @brief Get parser error code from output.
 *
 * @param out Parse output.
 *
 * @return Error code (`enum e_args_error`).
 */
int	args_error(
	const t_args_output *out
);

/**
 * @brief Convert parser error code to string.
 *
 * @param err Error code.
 *
 * @return Static error description string.
 */
const char	*args_error_str(
	int err
);

#endif	// INTERFACE_ARGS_H
/** @} */
