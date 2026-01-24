// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <ctype.h>
#include <stdarg.h>

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
	const t_param_args_type _args_spec,
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

/* -----| Parsing   |----- */
#pragma region Parsing

// /** */
// int				_args_parse(
// 					ARGSP *const _main,
// 					const int _argc,
// 					const char *_argv[]
// 				);

/* -----| Editions   |----- */
#pragma region Editions

// /** */
// // __attribute__((visibility("hidden")))
// int	_args_add_sub_parser(
// 	_t_args_parser *const parent,
// 	const char *const name,
// 	ARGSP **const dest
// );

// /** */
// // __attribute__((visibility("hidden")))
// int	_args_add_param_to_parser(
// 	_t_args_parser *const parent,
// 	const char *const name,
// 	ARGSP **const dest
// );

// /** */
// // __attribute__((visibility("hidden")))
// int	_args_add_param_to_option(
// 	_t_args_option *const parent,
// 	const char *const name,
// 	ARGSP **const dest
// );

// /** */
// // __attribute__((visibility("hidden")))
// int	_args_add_option(
// 	_t_args_parser *const parent,
// 	const char *const lname,
// 	const char sname,
// 	ARGSP **const dest
// );

// /** */
// // __attribute__((visibility("hidden")))
// int	_args_set_desc_parser(
// 	_t_args_parser *const target,
// 	const enum _e_args_set_desc_actions _action,
// 	const char *const _fmt,
// 	va_list *const _list
// );

// /** */
// // __attribute__((visibility("hidden")))
// int	_args_set_desc_option(
// 	_t_args_option *const target,
// 	const enum _e_args_set_desc_actions _action,
// 	const char *const _fmt,
// 	va_list *const _list
// );

// /** */
// // __attribute__((visibility("hidden")))
// int	_args_set_desc_param(
// 	_t_args_param *const target,
// 	const enum _e_args_set_desc_actions _action,
// 	const char *const _fmt,
// 	va_list *const _list
// );

// /* -----| Extraction |----- */
// #pragma region Extraction

// /** */
// // __attribute__((visibility("hidden")))
// int	_args_get_opt(
// 	_t_args_option *const opts,
// 	const char *name,
// 	void **dest
// );

// /** */
// // __attribute__((visibility("hidden")))
// int	_args_get_sub(
// 	_t_args_parser *const parsers,
// 	const char *name,
// 	void **dest
// );

// /** */
// // __attribute__((visibility("hidden")))
// int	_args_get_param(
// 	_t_args_param *const params,
// 	const char *name,
// 	void **dest
// );

/* -----| Checks     |----- */
#pragma region Checks

/** */
static inline int	_args_is_long_opt(
	const char *restrict _s
)
{
	const unsigned char	_c0 = _s ? _s[0] : '\0';
	const unsigned char	_c1 = _s ? _s[1] : '\0';
	const unsigned char	_c2 = _s ? _s[2] : '\0';

	const int			is_long = (_c0 == '-') & (_c1 == '-') & isalnum(_c2);
	const int			is_delim = (_c0 == '-') & (_c1 == '-') & (_c2 == '\0');

	return (is_long && !is_delim);
}

/** */
static inline int	_args_is_short_opt(
	const char *restrict _s
)
{
	const unsigned char	_c0 = _s ? _s[0] : '\0';
	const unsigned char	_c1 = _s ? _s[1] : '\0';
	const unsigned char	_c2 = _s ? _s[2] : '\0';

	const int			is_short = (_c0 == '-') & isalpha(_c1) & (_c2 == '\0');
	const int			is_delim = (_c0 == '-') & (_c1 == '-') & (_c2 == '\0');

	return (is_short && !is_delim);
}

/** */
static inline int	_args_is_opt(
	const char *restrict _s
)
{
	return (_s && (_args_is_short_opt(_s) || _args_is_long_opt(_s)));
}

// /** */
// // __attribute__((visibility("hidden")))
// bool			_args_check_sub_exist_parser(
// 					const _t_args_parser *const restrict parser,
// 					const char *const name
// 				);

// /** */
// // __attribute__((visibility("hidden")))
// bool			_args_check_opt_exist_parser(
// 					const _t_args_parser *const restrict parser,
// 					const char *name
// 				);

// /** */
// // __attribute__((visibility("hidden")))
// bool			_args_check_param_exist_parser(
// 					const _t_args_parser *const restrict parser,
// 					const char *name
// 				);

// /** */
// // __attribute__((visibility("hidden")))
// int				_args_check_parser(
// 					const _t_args_parser *const _parser
// 				);
