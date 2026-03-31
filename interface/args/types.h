/**
 * @file types.h
 * @brief Public types for the `interface/args` module.
 *
 * @ingroup args
 */

# pragma once

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */

/* -----| Enums     |----- */
typedef enum e_param_type				t_param_type;
typedef enum e_args_param_specs			t_args_param_specs;

/* -----| Structs   |----- */
typedef struct _s_args_param			t_args_param;
typedef struct _s_args_option			t_args_option;
typedef struct _s_args_parser			t_args_parser;
typedef struct _s_args_output_option	t_args_output_option;
typedef struct _s_args_output_parser	t_args_output_parser;
typedef struct _s_args_output			t_args_output;

/* ************************************************************************** */
/*                                  Enums                                     */
/* ************************************************************************** */

/**
 * @brief Logical type declared for a parameter.
 *
 * @note These are metadata hints stored in parser definitions. The parser does
 *       not automatically cast values.
 */
enum e_param_type
{
	param_type_file,	/**< File path or filename string. */
	param_type_int,		/**< Signed integer semantic. */
	param_type_uint,	/**< Unsigned integer semantic. */
	param_type_str,		/**< Generic string semantic. */
};

/**
 * @brief Parameter behavior flags.
 */
enum e_args_param_specs
{
	args_param_specs_require = 1 << 0,	/**< Parameter is required. */
	args_param_specs_nargs =   1 << 1,	/**< Parameter can hold multiple values. */
};

/**
 * @brief Parser error codes.
 */
enum e_args_error
{
	args_error_none = 0,				/**< No error. */

	/* internal / programmer errors */
	args_error_internal,				/**< Internal misuse or model/allocation failure path. */

	/* option / command resolution */
	args_error_unknown_option,			/**< Unknown option token. */
	args_error_ambiguous_option,		/**< Ambiguous/invalid option declaration. */
	args_error_unknown_command,			/**< Unknown subcommand token. */

	/* argument structure */
	args_error_missing_param,			/**< Required parameter is missing. */
	args_error_extra_param,				/**< Too many positional arguments. */
	args_error_missing_value,			/**< Required option value is missing. */

	/* value validation */
	args_error_invalid_type,			/**< Value type check failed. */
	args_error_out_of_range,			/**< Numeric range check failed. */
	args_error_invalid_value,			/**< Generic semantic value check failed. */

	/* usage / logic */
	args_error_conflict,				/**< Conflicting declarations or usage. */
	args_error_duplicate,				/**< Duplicated declaration or usage. */
};
