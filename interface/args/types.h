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
	//...

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */

/* -----| Typename  |----- */
	//...

/* -----| Enums     |----- */
typedef enum e_param_type			t_param_type;

/* -----| Structs   |----- */

typedef struct s_args_param			t_args_param;
typedef struct s_args_option		t_args_option;
typedef struct s_args_parser		t_args_parser;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

enum e_param_type
{
	param_type_file,
	param_type_int,
	param_type_uint,
	param_type_str,
	param_type_error,
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
struct s_args_param
{
	char			*name;		// naem of the parameter
	char			*desc;		// description of the parameter
	t_args_param	*next;		// next param in the context
	t_param_type	type;		// type of the expected data
	char			mandatory;	// is this param mandatory
};

/**
 * @struct	args_option
 * 
 * @brief	struct to store data for a option (like --help/-h)
 */
struct s_args_option
{
	char			*long_name;	// long name of the option (only one char == only short option)
	char			*desc;		// description of the option
	t_args_param	*params;	// params of the option
	t_args_option	*next;		// next possible option the context
};

/**
 * @brief	args_parser
 * 
 * @brief	store data for the app parser
 */
struct s_args_parser
{
	t_args_param	*params;		// params of the parser
	t_args_option	*options;		// option of the parser/context
	t_args_parser	*sub_parsers;	// sub parser of the context
	char			*desc;			// description of this context/app
};
