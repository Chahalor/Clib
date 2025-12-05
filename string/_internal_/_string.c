// Header

/* ----| Headers    |----- */
	/* Standard */
		//...
	/* Internal */
#include "_string.h"
	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

int	_char_strlen(
	const char *const _str
)
{
	register int	i = 0;

	if (unlikely(!_str))
		return (0);

	while (_str[i])
		i++;
	return (i);
}

int	_str_strlen(
	const t_str *const _str
)
{
	if (unlikely(!_str))
		return (0);
	else
		return (_str->len);
}
