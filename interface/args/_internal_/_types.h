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

// typedef struct _s_args_map_entry		_t_args_map_entry;
// typedef struct _s_args_map				_t_args_map;

typedef struct _s_args_config			_t_args_config;

typedef struct _s_args_param			_t_args_param;
typedef struct _s_args_option			_t_args_option;
typedef struct _s_args_parser			_t_args_parser;
typedef struct _s_args_root				_t_args_root;

typedef struct _s_args_output_value		_t_args_output_value;
typedef struct _s_args_output_param		_t_args_output_param;
typedef struct _s_args_output_option	_t_args_output_option;
typedef struct _s_args_output_parser	_t_args_output_parser;


/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

enum e_args_context_type
{
	args_context_parser,
	args_context_opt,
	args_context_param,
};

enum _e_args_set_desc_actions
{
	_args_set_desc_add,
	_args_set_desc_rm,
};

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

// struct _s_args_map_entry
// {
// 	char				*key;
// 	char				*value;
// 	_t_args_map_entry	*next;
// };

// struct _s_args_map
// {
// 	_t_args_map_entry	**bucket;
// 	size_t				nb_entry;
// 	size_t				capacity;
// };

struct _s_args_config
{
	/* behavior */
	char	exit_on_error			: 1;
	char	exit_on_help			: 1;
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


/**
 * @struct	args_param
 * 
 * @brief	struct to store data for param like a filename or a value
 */
struct _s_args_param
{	
	t_param_type		type;		// type of the expected data
	t_param_args_type	args_type;	// type of the args content (requiered, nargs, ...)

	char				*name;		// name of the parameter
	char				*desc;		// description of the parameter

	_t_args_param		*next;		// next param in the context
};

/**
 * @struct	args_option
 * 
 * @brief	struct to store data for a option (like --help/-h)
 */
struct _s_args_option
{
	char			short_name;	// short name of the option
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

struct _s_args_root
{
	_t_args_parser	*parser;		// main parser struct
	_t_args_config	config;			// config of this parser
};


#pragma region Output

struct _s_args_output_value
{
	char					*value;
	_t_args_output_value	*next;
};


struct _s_args_output_param
{
	int						error;
	char					*name;
	_t_args_output_value	*values;
	_t_args_output_param	*next;
};


struct s_args_output_option
{
	char					short_name;
	unsigned int			nb_call;
	int						error;

	char					*long_name;
	_t_args_param			*params;
	_t_args_output_option	*next;
};

struct _s_args_output_parser
{
	char					*name;

	_t_args_output_option	*options;
	_t_args_output_param	*params;

	_t_args_output_parser	*sub;
	_t_args_output_parser	*next;
};

struct _s_args_output
{
	int						error;
	_t_args_output_parser	*root;
};
