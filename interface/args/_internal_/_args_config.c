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

// __attribute__((visibility("hidden")))
_t_args_config	*_args_config_get(void)
{
	static _t_args_config	config = {
		.format_cmd = "\t%20s%s\n",
		.sub_format = "\t%20s%s\n"
	};

	return (&config);
}

// __attribute__((visibility("hidden")))
void	_args_update_config(
	const char _field,
	const bool _value
)
{
	// switch (expression)
	// {
	// case (constant expression):
	// 	/* code */
	// 	break;
	
	// default:
	// 	break;
	// }
}

// __attribute__((visibility("hidden")))
void	_args_config_set_errnum(
	const int _error
)
{
	_t_args_config	*const restrict _config = _args_config_get();

	_config->errnum = _error;

	// if (_error && _config->exit_on_error)
	// 	_args_exit(_args_strerror(_error));
}
