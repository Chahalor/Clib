// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "_internal_/_args.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

int	args_error(
	const t_args_output *out
)
{
	if (!out)
		return (args_error_internal);
	else
		return (out->error);
}


const char	*args_error_str(
	const int err
)
{
	return (_args_error_str(err));
}
