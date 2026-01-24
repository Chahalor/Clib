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

// /**
//  * @brief	allocate and initialize a `_t_args_param` descriptor
//  *
//  * @param	_name	optional name used only to size the inline buffer
//  *
//  * @return	the newly allocated param struct or NULL on error
//  *
//  * @note	the name buffer is part of the allocation and is not populated
//  * @note	.desc is left NULL and must be allocated separately if needed
//  * @version	1.0.0
// */
// ARGSP	*_args_mem_new_param(
// 	const char *const restrict _name
// );

// /**
//  * @brief	allocate and initialize a `_t_args_option` descriptor
//  *
//  * @param	_name	optional long name used only to size the inline buffer
//  *
//  * @return	the newly allocated option struct or NULL on error
//  *
//  * @note	the long-name buffer is part of the allocation and is not populated
//  * @note	.desc is left NULL and must be allocated separately if needed
//  * @version	1.0.0
//  */
// ARGSP	*_args_mem_new_option(
// 	const char *const restrict _name,
// 	const char _short_name
// );

// /**
//  * @brief	allocate and initialize a `_t_args_parser` descriptor
//  *
//  * @param	_name	optional parser name used only to size the inline buffer
//  *
//  * @return	the newly allocated parser struct or NULL on error
//  *
//  * @note	the name buffer is part of the allocation and is not populated
//  * @note	.desc, .params, .options, and .sub_parsers are set to NULL
//  * @version	1.0.0
//  */
// ARGSP	*_args_mem_new_parser(
// 	const char *const restrict _name
// );

// /**
//  * TODO: Docs
//  */
// ARGSP	*_args_mem_new_root(void);

// /**
//  * @brief	free a parameter descriptor
//  *
//  * @param	_param		parameter to release (must be non-NULL)
//  * @param	_recursiv	if nonzero, free the full linked list via .next
//  *
//  * @version	1.0.0
//  */
// void	_args_mem_free_param(
// 	ARGSP *const restrict _param,
// 	const char _recursiv
// );

// /**
//  * @brief	free an option descriptor
//  *
//  * @param	_param		option to release (must be non-NULL)
//  * @param	_recursiv	if nonzero, free the full linked list via .next
//  *
//  * @version	1.0.0
//  */
// void	_args_mem_free_opt(
// 	ARGSP *const restrict _param,
// 	const char _recursiv
// );

// /**
//  * @brief	free a parser descriptor and its owned children
//  *
//  * @param	_param		parser to release (must be non-NULL)
//  * @param	_recursiv	if nonzero, free the full linked list via .next
//  *
//  * @note	also frees .params, .options, and .sub_parsers lists
//  * @version	1.0.0
//  */
// void	_args_mem_free_parser(
// 	ARGSP *const restrict _param,
// 	const char _recursiv
// );

// /** */
// void	_args_remove_sub(
// 	ARGSP *const _main,
// 	ARGSP *const _sub
// );

// /** */
// void	_args_remove_opt(
// 	ARGSP *const _main,
// 	ARGSP *const _opt
// );

// /** */
// void	_args_remove_param_from_opt(
// 	ARGSP *const _main,
// 	ARGSP *const _param
// );

// /** */
// void	_args_remove_param_from_parser(
// 	ARGSP *const _main,
// 	ARGSP *const _param
// );

// /* -----| Parsing   |----- */
// #pragma region Parsing

// /** */
// int				_args_parse(
// 					ARGSP *const _main,
// 					const int _argc,
// 					const char *_argv[]
// 				);

// /* -----| Editions   |----- */
// #pragma region Editions

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

// /* -----| Checks     |----- */
// #pragma region Checks

// /** */
// static inline int	_args_is_long_opt(
// 	const char *restrict _s
// )
// {
// 	const unsigned char	_c0 = _s[0];
// 	const unsigned char	_c1 = _s[1];
// 	const unsigned char	_c2 = _s[2];

// 	const int			is_long  = (_c0 == '-') & (_c1 == '-') & isalnum(_c2);
// 	const int			is_delim = (_c0 == '-') & (_c1 == '-') & (_c2 == '\0');

// 	return (is_long && !is_delim);
// }

// /** */
// static inline int	_args_is_short_opt(
// 	const char *restrict _s
// )
// {
// 	const unsigned char	_c0 = _s[0];
// 	const unsigned char	_c1 = _s[1];
// 	const unsigned char	_c2 = _s[2];

// 	const int			is_short = (_c0 == '-') & isalpha(_c1) & (_c2 == '\0');
// 	const int			is_delim = (_c0 == '-') & (_c1 == '-') & (_c2 == '\0');

// 	return (is_short && !is_delim);
// }

// /** */
// static inline int	_args_is_opt(
// 	const char *restrict _s
// )
// {
// 	return (_args_is_short_opt(_s) || _args_is_long_opt(_s));
// }

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
