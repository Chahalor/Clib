// Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdarg.h>

	/* Internal */
#include "_args.h"

	/* External */
#include "_internal_/_internal_.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static inline int	_set_desc(
	char **const _target,
	const char *const _desc
)
{
	char	*_new;
	size_t	_length_desc;
	int		result = error_none;

	if (_desc)
	{
		_length_desc = strlen(_desc);
		_new = mem_alloc(sizeof(char) * (_length_desc + 1));
		if (unlikely(!_new))
		{
			result = error_alloc_fail;
			goto error;
		}
		mem_free(*_target);
		strcpy(_new, _desc);
		*_target = _new;
	}
	else
	{
		mem_free(*_target);
		*_target = NULL;
	}

error:
	return (result);
}

/* ----| Public     |----- */

int	_args_parser_set_desc(
	_t_args_parser *const _target,
	const char *const _desc
)
{
	return (_set_desc(&_target->desc, _desc));
}

int	_args_param_set_desc(
	_t_args_param *const _target,
	const char *const _desc
)
{
	return (_set_desc(&_target->desc, _desc));

}

int	_args_option_set_desc(
	_t_args_option *const _target,
	const char *const _desc
)
{
	return (_set_desc(&_target->desc, _desc));
}