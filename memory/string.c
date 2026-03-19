/**
 * 
*/

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "lib_config.h"
# include "lib_standards.h"

/* -----| Internals |----- */
# include "string/types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

/**
 * @brief	allocate a new string
*/
#define	string_new(var)	_Generic(\
	size_t:					string_new_size(var), \
	const char *const:		string_new_char_ptr(var), \
	char:					string_new_char(var), \
	const string *const:	string_new_string_ptr(var), \
	const string:			string_new_string(var) \
)

#define stri

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/* -----| Memory    |----- */
//--- alloc ---//

string	*string_new_size(
	const size_t size
);

string	*string_new_char_ptr(
	const char *const str
);

string	*string_new_char(
	const char c
);

string	*string_new_string_ptr(
	const char *const string
);

string	*string_new_string(
	const char string
);

//--- free ---//

void	string_free(
	const string *const string
);

//--- access ---//

char	string_access_index