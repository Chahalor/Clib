// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
	//...

/* -----| Internals |----- */
#include "../types.h"

/* -----| Modules   |----- */
	//...


/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */

/* -----| Typename  |----- */
	//...

/* -----| Enums     |----- */
	//...

/* -----| Structs   |----- */

typedef struct _s_args_param		_t_args_param;
typedef struct _s_args_option		_t_args_option;
typedef struct _s_args_parser		_t_args_parser;
typedef struct _s_args_config		_t_args_config;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

enum e_param_type
{
	param_type_file,
	param_type_int,
	param_type_uint,
	param_type_str,
};

enum e_param_args_type
{
	param_args_type_require = 0x1,	// this param is required
	param_args_type_nargs = 0x10,	// this param take n args
	// param_args_type_action = 0x111,	// this param need an action
};

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

/**
 * @struct	args_param
 * 
 * @brief	struct to store data for param like a filename or a value
 */
struct _s_args_param
{	
	char				*name;		// naem of the parameter
	char				*desc;		// description of the parameter
	_t_args_param		*next;		// next param in the context
	t_param_type		type;		// type of the expected data
	t_param_args_type	args_type;	// type of the args content (requiered, nargs, ...)
};

/**
 * @struct	args_option
 * 
 * @brief	struct to store data for a option (like --help/-h)
 */
struct _s_args_option
{
	char			*long_name;	// long name of the option (only one char == only short option)
	char			*desc;		// description of the option
	_t_args_param	*params;	// params of the option
	_t_args_option	*next;		// next possible option the context
};

/**
 * @brief	args_parser
 * 
 * @brief	store data for the app parser
 */
struct _s_args_parser
{
	char			*name;			// name of the context/sub-parser
	char			*desc;			// description of this context/app
	_t_args_param	*params;		// params of the parser
	_t_args_option	*options;		// option of the parser/context
	_t_args_parser	*sub_parsers;	// sub parser of the context
	_t_args_parser	*next;			// next sub-parser for the current context
};

struct _s_args_config
{
	/* behavior */
	char	exit_on_error			: 1;
	char	auto_help				: 1;
	char	auto_usage				: 1;

	/* param checks */
	char	check_required_param	: 1;
	char	check_unknown_option	: 1;
	char	check_duplicate_option	: 1;
	char	check_conflict_option	: 1;

	/* file checks */
	char	check_file_exists		: 1;
	char	check_file_read			: 1;
	char	check_file_write		: 1;

	/* numeric checks */
	char	check_nbr_overflow		: 1;
	char	check_nbr_range			: 1;

	/* string checks */
	char	check_str_empty			: 1;
	char	check_str_length		: 1;
};
