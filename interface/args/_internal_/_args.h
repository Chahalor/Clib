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
	//...

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/**
 * @brief	return the args parser config struct
 */
_t_args_config	*_args_get_config(void);

/* -----| Memory    |----- */

/** */
_t_args_param	*_args_mem_new_param(
					const char *const restrict _name
					);

/** */
_t_args_option	*_args_mem_new_option(
					const char *const restrict _name
					);

/** */
_t_args_parser	*_args_mem_new_parser(
					const char *const restrict _name
					);

/** */
void			_args_mem_free_param(
					_t_args_param *const restrict _param,
					const char _recursiv
					);

/** */
void			_args_mem_free_opt(
					_t_args_option *const restrict _param,
					const char _recursiv
					);

/** */
void			_args_mem_free_parser(
					_t_args_parser *const restrict _param,
					const char _recursiv
					);