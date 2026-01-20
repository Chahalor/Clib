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
typedef enum e_param_args_type		t_param_args_type;

/* -----| Structs   |----- */

typedef struct _s_args_param		t_args_param;
typedef struct _s_args_option		t_args_option;
typedef struct _s_args_parser		t_args_parser;

typedef struct s_args_output_param	t_args_output_param;
typedef struct s_args_output_option	t_args_output_option;
typedef struct s_args_output		t_args_output;

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
	param_args_type_require = 1 << 0,	// this param is required
	param_args_type_nargs =   1 << 1,		// this param take n args
};

enum e_args_error
{
	args_error_none = 0,

	/* internal / programmer errors */
	args_error_internal,			// NULL ptr, alloc fail, inconsistent model

	/* option / command resolution */
	args_error_unknown_option,		// --wat
	args_error_ambiguous_option,	// --f matches --file and --force
	args_error_unknown_command,		// sub-parser not found

	/* argument structure */
	args_error_missing_param,		// required param missing
	args_error_extra_param,			// too many params
	args_error_missing_value,		// option expects a value but none provided

	/* value validation */
	args_error_invalid_type,		// "abc" for int
	args_error_out_of_range,		// overflow / bounds
	args_error_invalid_value,		// semantic error (eg negative when forbidden)

	/* usage / logic */
	args_error_conflict,			// mutually exclusive options
	args_error_duplicate,			// option not repeatable but used twice
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
struct s_args_output_param
{	
	char				*name;		// name of the parameter
	char				*data;		// data get for the argc/v, store has a string
	t_args_output_param	*next;		// next param in the context
	t_param_type		type;		// type of the expected data
	unsigned int		nb_data;	// number fo data passed to the args
};

/**
 * @struct	args_option
 * 
 * @brief	struct to store data for a option (like --help/-h)
 */
struct s_args_output_option
{
	char					*long_name;	// long name of the option (only one char == only short option)
	t_args_output_param		*params;	// params of the option
	t_args_output_option	*next;		// next possible option the context
	unsigned int			nb_call;	// number of time this option has been call
};

/**
 * @brief	args_parser
 * 
 * @brief	store data for the app parser
 */
struct s_args_output
{
	char					*name;		// name of the context
	t_args_output_param		*params;	// params of the parser
	t_args_output_option	*options;	// option of the parser/context
	t_args_output			*sub;		// sub parser of the context
};
