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
typedef enum e_param_type			t_param_type;		// TODO: docs
typedef enum e_param_args_type		t_param_args_type;	//

/* -----| Structs   |----- */
typedef struct _s_args_parser_root	ARGSP;				//
typedef ARGSP						ARGS_PARAM;			//
typedef ARGSP						ARGS_OPT;			//
typedef ARGSP						ARGS_SUB_PARSER;	//

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
	param_args_type_nargs =   1 << 1,	// this param take n args
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
	//...
