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

__attribute__((visibility("hidden")))
_t_args_config	*_args_get_config(void)
{
	static _t_args_config	config = {0};

	return (&config);
}
