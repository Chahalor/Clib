// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "config.h"
# include "standards.h"

/* -----| Internals |----- */
# include "types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

#define strLen(_str)	_Generic((_str), \
							char *:		_char_strlen(_str), \
							t_string *:	_str_strlen(_str));

#define strCat(dest, src) _Generic((dest), \
								char*: _Generic((src), \
									char*:		_charstr_cat, \
									t_string *:	_char_tstr_cat \
								), \
								t_string *: _Generic((src), \
									char*:		_tstr_char_cat, \
									t_string *:	_strstr_cat \
								) \
							)(dest, src)

#define strCmp(_s1, _s2)	_Generic((_s1), \
								char *: _Generic((_s2), \
									char *:		_charstr_cmp(_s1, _s2), \
									t_string *:	_strstr_cmp(_s1, _s2) \
								), \
								t_string *: _Generic((_s2), \
									char*: 		_strchar_cmp(_s1, _s2), \
									t_string*:	_strstr_cmp(_s1, _s2) \
								) \
							)(_s1, _s2) 


/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */
	//...