// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>	// rm

/* -----| Globals   |----- */
# include "lib_config.h"
# include "lib_standards.h"

/* -----| Internals |----- */
# include "_types.h"
# include "../../args.h"

/* -----| Modules   |----- */
#include "memory.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

#define _ARGS_PARSE_PARAM_ALLOC_SIZE	8

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/* -----| Config    |----- */
#pragma region Config

/**
 * @brief	return the args parser configuration singleton
 *
 * @return	pointer to the module-owned config structure
 *
 * @version	1.0.0
 */
_t_args_config	*_args_config_get(void);

/**
 * @brief	set the module error number in the config singleton
 *
 * @param	_error	error code to store
 *
 * @return	void
 */
// __attribute__((visibility("hidden")))
void			_args_config_set_errnum(
					const int _error
					);

/* -----| Memory    |----- */
#pragma region Memory

/**
 * @brief	initialize the root args configuration with defaults
 *
 * @param	_conf	target config structure to initialize
 *
 * @return	void
 */
void	_args_init_root_config(
	_t_args_config *const _conf
);

/**
 * @brief	allocate and initialize a new parser node
 *
 * @param	_name	parser name (optional)
 * @param	_desc	parser description (optional)
 *
 * @return	new parser instance or NULL on allocation failure
 */
_t_args_parser	*_args_mem_new_parser(
	const char *const _name,
	const char *const _desc
);

/**
 * @brief	allocate and initialize a new option node
 *
 * @param	_long_name	option long name (optional)
 * @param	_short_name	option short name (0 if unused)
 * @param	_desc		option description (optional)
 *
 * @return	new option instance or NULL on allocation failure
 */
_t_args_option	*_args_mem_new_option(
	const char *const _long_name,
	const char _short_name,
	const char *const _desc
);

/**
 * @brief	allocate and initialize a new parameter node
 *
 * @param	_name		parameter name (optional)
 * @param	_desc		parameter description (optional)
 * @param	_args_spec	parameter arguments specification
 * @param	_type		parameter value type
 *
 * @return	new parameter instance or NULL on allocation failure
 */
_t_args_param	*_args_mem_new_param(
	const char *const _name,
	const char *const _desc,
	const t_args_param_specs _args_spec,
	const t_param_type _type
);

/**
 * @brief	allocate and initialize a new output value node
 *
 * @param	_value	string value to store (optional)
 *
 * @return	new output value instance or NULL on allocation failure
 */
_t_args_output_value	*_args_mem_new_out_val(
	const char *const _value
);

/**
 * @brief	allocate and initialize a new output parameter node
 *
 * @param	_name	parameter name (optional)
 *
 * @return	new output parameter instance or NULL on allocation failure
 */
_t_args_output_param	*_args_mem_new_out_param(
	const char *const _name
);

/**
 * @brief	allocate and initialize a new output option node from a template option
 *
 * @param	_opt	source option used to populate names and flags
 *
 * @return	new output option instance or NULL on allocation failure
 */
_t_args_output_option	*_args_mem_new_out_opt(
	const _t_args_option *const _opt
);

/**
 * @brief	allocate and initialize a new output parser node
 *
 * @param	_name	parser name (optional)
 *
 * @return	new output parser instance or NULL on allocation failure
 */
_t_args_output_parser	*_args_mem_new_out_parser(
	const char *const _name
);

/**
 * @brief	allocate and initialize a new output container
 *
 * @return	new output container or NULL on allocation failure
 */
_t_args_output	*_args_mem_new_output(void);

/**
 * @brief	free a parser definition tree
 *
 * @param	_parser	parser node (root of list)
 * @param	_deep	free nested lists when true
 *
 * @return	void
 */
void	_args_mem_free_parser(
	_t_args_parser *const _parser,
	const bool _deep
);

/**
 * @brief	free a parsed output tree
 *
 * @param	_out	output container
 * @param	_deep	free nested lists when true
 *
 * @return	void
 */
void	_args_mem_free_output(
	_t_args_output *const _out,
	const bool _deep
);

/* -----| Parsing   |----- */
#pragma region Parsing

/**
 * @brief	parse argv and build an output tree from the parser definition
 *
 * @param	_parser	root parser definition
 * @param	_argc	argument count
 * @param	_argv	argument vector
 *
 * @return	output tree or NULL on allocation failure
 */
_t_args_output	*_args_parse(
	const t_args_parser *const _parser,
	const int _argc,
	const char *const *const _argv
);

/* -----| Editions   |----- */
#pragma region Editions

// /** */
// __attribute__((visibility("hidden")))
/**
 * @brief	set or clear a parser description string
 *
 * @param	_target	parser to update
 * @param	_desc	new description (NULL to clear)
 *
 * @return	error_none on success or error_alloc_fail on allocation failure
 */
int	_args_parser_set_desc(
	_t_args_parser *const _target,
	const char *const _desc
);

// /** */
// __attribute__((visibility("hidden")))
/**
 * @brief	set or clear a parameter description string
 *
 * @param	_target	param to update
 * @param	_desc	new description (NULL to clear)
 *
 * @return	error_none on success or error_alloc_fail on allocation failure
 */
int	_args_param_set_desc(
	_t_args_param *const _target,
	const char *const _desc
);

// /** */
// __attribute__((visibility("hidden")))
/**
 * @brief	set or clear an option description string
 *
 * @param	_target	option to update
 * @param	_desc	new description (NULL to clear)
 *
 * @return	error_none on success or error_alloc_fail on allocation failure
 */
int	_args_option_set_desc(
	_t_args_option *const _target,
	const char *const _desc
);

/** */
t_args_parser	*_args_parser_add_sub(
	t_args_parser *const _parent,
	const char *const _name,
	const char *const _desc
);

/** */
t_args_option	*_args_parser_add_option(
	t_args_parser *const _parent,
	const char *const _long_name,
	const char _sname,
	const char *const _desc
);

/** */
t_args_param	*_args_parser_add_param(
	t_args_parser *const _parent,
	const char *const _name,
	const char *const _desc,
	const int _spec,
	const int _type
);

/** */
t_args_param	*_args_option_add_param(
	t_args_option *const _parent,
	const char *const _name,
	const char *const _desc,
	const int _spec,
	const int _type
);

/* -----| Extraction |----- */
#pragma region Extraction

/**
 * @brief	get values for a named output parameter
 *
 * @param	_output	parsed output tree
 * @param	_name	parameter name
 * @param	_values	receives array of value pointers (allocated)
 * @param	_count	receives number of values
 *
 * @return	parameter name if found, NULL otherwise
 */
char	*_args_get_param(
	t_args_output *const _output,
	const char *const _name,
	char *const * *const _values,
	unsigned int *const _count
);

/**
 * @brief	get values for a named output parameter from a sub-parser output
 *
 * @param	_output	sub-parser output node
 * @param	_name	parameter name
 * @param	_values	receives array of value pointers (allocated)
 * @param	_count	receives number of values
 *
 * @return	parameter name if found, NULL otherwise
 */
char	*_args_output_parser_get_param(
	t_args_output_parser *const _output,
	const char *const _name,
	char *const * *const _values,
	unsigned int *const _count
);

/** */
/**
 * @brief	get option values merged across all params for a named option
 * if the option has no param, `_count` is set to 0 and `_values[0]` is set to `0x1`
 *
 * @param	_output	parsed output tree
 * @param	_name	option name (short/long or raw)
 * @param	_values	receives merged values pointer
 * @param	_count	receives number of values
 *
 * @return	currently returns 0 (reserved)
 */
char	_args_get_option(
	t_args_output *const _output,
	const char *const _name,
	char *const * *const _values,
	unsigned int *const _count
);

/**
 * @brief	get option values for a named option from a sub-parser output
 *
 * @param	_output	sub-parser output node
 * @param	_name	option name (short/long or raw)
 * @param	_values	receives merged values pointer
 * @param	_count	receives number of values
 *
 * @return	non-zero if found, zero otherwise
 */
char	_args_output_parser_get_option(
	t_args_output_parser *const _output,
	const char *const _name,
	char *const * *const _values,
	unsigned int *const _count
);

/** */
/**
 * @brief	return the active subcommand name, if any
 *
 * @param	_out	parsed output tree
 *
 * @return	subcommand name or NULL
 */
const char	*_args_active_subcommand(
	const t_args_output *_out
);

/** */
/**
 * @brief	return the output parser for the first subcommand
 *
 * @param	_out	parsed output tree
 *
 * @return	subcommand output parser or NULL
 */
_t_args_output_parser	*_args_get_sub_output(
	const t_args_output *_out
);


/* -----| Checks     |----- */
#pragma region Checks

/**
 * @brief	check if a string is a long option ("--name")
 *
 * @param	_s	candidate string
 *
 * @return	non-zero if long option, zero otherwise
 */
static inline int	_args_is_long_opt(
	const char *restrict _s
)
{
	const unsigned char	_c0 = _s ? _s[0] : '\0';
	const unsigned char	_c1 = _s ? _s[1] : '\0';
	const unsigned char	_c2 = _s ? _s[2] : '\0';

	const int			is_long = (_c0 == '-') && (_c1 == '-') && isalnum(_c2);
	const int			is_delim = (_c0 == '-') && (_c1 == '-') && (_c2 == '\0');

	return (is_long && !is_delim);
}

/**
 * @brief	check if a string is a short option ("-n")
 *
 * @param	_s	candidate string
 *
 * @return	non-zero if short option, zero otherwise
 */
static inline int	_args_is_short_opt(
	const char *restrict _s
)
{
	const unsigned char	_c0 = _s ? _s[0] : '\0';
	const unsigned char	_c1 = _s ? _s[1] : '\0';
	const unsigned char	_c2 = _s ? _s[2] : '\0';

	const int			is_short = (_c0 == '-') && isalpha(_c1) && (_c2 == '\0');
	const int			is_delim = (_c0 == '-') && (_c1 == '-') && (_c2 == '\0');

	return (is_short && !is_delim);
}

/** */
static inline bool	_args_param_is_requiered(
	const _t_args_param *const _param
)
{
	return (_param->specs & args_param_specs_require);
}

static inline bool	_args_param_has_nargs(
	const _t_args_param *const _param
)
{
	return (_param->specs & args_param_specs_nargs);
}

/**
 * @brief	check if a string is any option ("-n" or "--name")
 *
 * @param	_s	candidate string
 *
 * @return	non-zero if option, zero otherwise
 */
static inline int	_args_is_opt(
	const char *restrict _s
)
{
	return (_s && (_args_is_short_opt(_s) || _args_is_long_opt(_s)));
}

/** */
/**
 * @brief	check if the parsed output has a parameter name
 *
 * @param	_output	parsed output tree
 * @param	_name	parameter name
 *
 * @return	true if found, false otherwise
 */
bool	_args_parser_has_param(
	const t_args_output *const _output,
	const char *const _name
);

/** */
/**
 * @brief	check if the parsed output has an option name
 *
 * @param	_output	parsed output tree
 * @param	_name	option name (short/long or raw)
 *
 * @return	true if found, false otherwise
 */
bool	_args_parser_has_option(
	const t_args_output *const _output,
	const char *const _name
);

/** */
/**
 * @brief	check if the parsed output has a subcommand by name
 *
 * @param	_output	parsed output tree
 * @param	_name	subcommand name
 *
 * @return	true if found, false otherwise
 */
char	_args_parser_has_sub(
	const t_args_output *const _output,
	const char *const _name
);

/** */
/**
 * @brief	check if an output option has a parameter by name
 *
 * @param	_option	output option node
 * @param	_name	parameter name
 *
 * @return	true if found, false otherwise
 */
char	_args_option_has_param(
	const t_args_output_option *const _option,
	const char *const _name
);

/**
 * @brief	validate parsed output against the parsing root
 *
 * @param	_root	parser runtime context
 * @param	_output	parsed output tree
 *
 * @return	error_none on success, otherwise an error code
 */
int	_args_check_output(
	const _t_args_root *const _root,
	const _t_args_output *const _output
);

#pragma region Builtins

int	_args_builtin_help(
	const _t_args_parser *const _parser,
	const _t_args_config *const _config
);


#pragma region Errors

const char	*_args_error_str(
	const int _errnum
);
