// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "../args.h"
#include "_internal_/_args.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */


ARGSP		*args_parser_new(void)
{
	return (_args_mem_new_parser(NULL));
}

void	args_free_elt(
	ARGSP *const _elt
)
{
	int	result = error_none;

	if (unlikely(!_elt))
	{
		result = error_invalid_arg;
		goto error;
	}

	switch (_elt->type)
	{
	case (_e_args_data_type_parser):
	case (_e_args_data_type_root):
	{
		_args_mem_free_param(_elt, true);
		break;
	}
	case (_e_args_data_type_opt):
		_args_mem_free_opt(_elt, true);
		break;
	case (_e_args_data_type_param):
		_args_mem_free_param(_elt, true);
		break;
	default:
		result = error_invalid_arg;
	}

error:
	return /* (result) */;
}

void	args_remove_sub(
	ARGSP *const _main,
	ARGS_SUB_PARSER *const _sub
)
{
	if (unlikely(!_main || !_sub ||
		(_main->type != _e_args_data_type_parser && _main->type != _e_args_data_type_root)))
		_args_config_set_errnum(error_invalid_arg);
	else
		_args_remove_sub(_main, _sub);

}

void	args_remove_option(
	ARGSP *const _main,
	ARGS_OPT *const _opt
)
{
	if (unlikely(!_main || !_opt ||
		(_main->type != _e_args_data_type_parser && _main->type != _e_args_data_type_root)))
		_args_config_set_errnum(error_invalid_arg);
	else
		_args_remove_opt(_main, _opt);
}

void	args_remove_param(
	ARGSP *const _main,
	ARGS_PARAM *const _param
)
{
	if (unlikely(!_main || !_param || _main->type == _e_args_data_type_param))
		_args_config_set_errnum(error_invalid_arg);
	else if (_main->type == _e_args_data_type_opt)
		_args_remove_param_from_opt(_main, _param);
	else
		_args_remove_param_from_parser(_main, _param);
}

void			args_destroy_parser(
	ARGSP *const restrict parser
)
{
	_args_mem_free_parser(parser, true);
}
