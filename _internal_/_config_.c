// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "_config_.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

_t_internal_config_	*_internal_get_config_(void)
{
	static _t_internal_config_ config = {
		.errnum = 0,
		.string_alloc_size = 512
	};

	return (&config);
}
