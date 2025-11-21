// Header

/* ----| Headers    |----- */
	/* Standard */
		//...
	/* Internal */
#include "_printf.h"
	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

int	_is_prec_flag(
	const char c
)
{
	return (c == '-' || c == '+' || c == ' ' || c == '#' || c == '0');
}

int	_is_length_flag(
	const char c
)
{
	return (c == 'h' || c == 'l' || c == 'j' || c == 'z' || c == 't' || c == 'L');
}

char	*_int_parser(
	const char *s,
	int *dest
)
{
	
}

/* ----| Public     |----- */

const char	*parse_format(
	const char *_format,
	t_fmt *out
)
{
	const char	*s = _format;

	*out = (t_fmt){0};
	out->precision = -1;

	while (_is_prec_flag(*s))
	{
		switch (*s)
		{
			case '-':
				out->flag_minus = 1;
				break;
			case '+':
				out->flag_plus = 1;
				break;
			case ' ':
				out->flag_space = 1;
				break;
			case '#':
				out->flag_hash = 1;
				break;
			case '0':
				out->flag_zero = 1;
				break;
			
			default:
				break;
		}
		s++;
	}

	if (*s == '*')
	{
		out->width = -2;
		s++;
	}
	else if (isdigit((unsigned char)*s))
		s = parse_number(s, &out->width);

	if (*s == '.')
	{
		s++;
		if (*s == '*')
		{
			out->precision = _printf_arg_arg; // indique que la précision vient d’un argument
			s++;
		}
		else
			s = parse_number(s, &out->precision);
	}

	if (_is_length_flag(*s))
	{
		out->length[0] = *s++;
		if ((*s == 'h' && out->length[0] == 'h') || (*s == 'l' && out->length[0] == 'l'))
			out->length[1] = *s++;
	}

	out->specifier = *s++;

	return (s);
}
