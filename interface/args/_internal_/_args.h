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
# include "_types.h"
# include "../../args.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

#define _ARGS_PARSE_PARAM_ALLOC_SIZE	8

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/* -----| Config    |----- */

/**
 * @brief	return the args parser configuration singleton
 *
 * @return	pointer to the module-owned config structure
 *
 * @version	1.0.0
 */
_t_args_config	*_args_config_get(void);

__attribute__((visibility("hidden")))
void			_args_config_set_errnum(
					const int _error
					);

/* -----| Memory    |----- */

/**
 * TODO: Docs
 */
_t_args_parser_root	*_args_mem_new_root(void);

/**
 * @brief	allocate and initialize a `_t_args_param` descriptor
 *
 * @param	_name	optional name used only to size the inline buffer
 *
 * @return	the newly allocated param struct or NULL on error
 *
 * @note	the name buffer is part of the allocation and is not populated
 * @note	.desc is left NULL and must be allocated separately if needed
 * @version	1.0.0
*/
_t_args_param	*_args_mem_new_param(
					const char *const restrict _name
					);

/**
 * @brief	allocate and initialize a `_t_args_option` descriptor
 *
 * @param	_name	optional long name used only to size the inline buffer
 *
 * @return	the newly allocated option struct or NULL on error
 *
 * @note	the long-name buffer is part of the allocation and is not populated
 * @note	.desc is left NULL and must be allocated separately if needed
 * @version	1.0.0
 */
_t_args_option	*_args_mem_new_option(
					const char *const restrict _name
					);

/**
 * @brief	allocate and initialize a `_t_args_parser` descriptor
 *
 * @param	_name	optional parser name used only to size the inline buffer
 *
 * @return	the newly allocated parser struct or NULL on error
 *
 * @note	the name buffer is part of the allocation and is not populated
 * @note	.desc, .params, .options, and .sub_parsers are set to NULL
 * @version	1.0.0
 */
_t_args_parser	*_args_mem_new_parser(
					const char *const restrict _name
					);

/**
 * @brief	free a parameter descriptor
 *
 * @param	_param		parameter to release (must be non-NULL)
 * @param	_recursiv	if nonzero, free the full linked list via .next
 *
 * @version	1.0.0
 */
void			_args_mem_free_param(
					_t_args_param *const restrict _param,
					const char _recursiv
					);

/**
 * @brief	free an option descriptor
 *
 * @param	_param		option to release (must be non-NULL)
 * @param	_recursiv	if nonzero, free the full linked list via .next
 *
 * @version	1.0.0
 */
void			_args_mem_free_opt(
					_t_args_option *const restrict _param,
					const char _recursiv
					);

/**
 * @brief	free a parser descriptor and its owned children
 *
 * @param	_param		parser to release (must be non-NULL)
 * @param	_recursiv	if nonzero, free the full linked list via .next
 *
 * @note	also frees .params, .options, and .sub_parsers lists
 * @version	1.0.0
 */
void			_args_mem_free_parser(
					_t_args_parser *const restrict _param,
					const char _recursiv
					);
