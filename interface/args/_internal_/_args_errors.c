// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "_args.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

const char	*_args_error_str(
	const int _errnum
)
{
	switch (_errnum)
	{
		case args_error_none:
			return ("no error");

		/* internal / programmer errors */
		case args_error_internal:
			return ("internal error");

		/* option / command resolution */
		case args_error_unknown_option:
			return ("unknown option");
		case args_error_ambiguous_option:
			return ("ambiguous option");
		case args_error_unknown_command:
			return ("unknown command");

		/* argument structure */
		case args_error_missing_param:
			return ("missing required parameter");
		case args_error_extra_param:
			return ("too many parameters");
		case args_error_missing_value:
			return ("missing value for option");

		/* value validation */
		case args_error_invalid_type:
			return ("invalid value type");
		case args_error_out_of_range:
			return ("value out of range");
		case args_error_invalid_value:
			return ("invalid value");

		/* usage / logic */
		case args_error_conflict:
			return ("conflicting options");
		case args_error_duplicate:
			return ("duplicate option");

		default:
			return ("unknown error");
	}
}
