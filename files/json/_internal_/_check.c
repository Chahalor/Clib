// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "_json.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

inline
int		_json_tool_is_sep(
	const char _c
)
{
	return (_c == '.' || _c == '/');
}

inline
int		_json_is_type_array(
	const int _type
)
{
	return (_type & json_tok_array);
}

inline
int		_json_is_type_obj(
	const int _type
)
{
	return (_type & json_tok_obj);
}

inline
int		_json_is_type_complex(
	const int _type
)
{
	return (_json_is_type_array(_type) || _json_is_type_obj(_type));
}

inline
int		_json_is_type_nbr(
	const int _type
)
{
	return (_type & json_tok_nbr);
}

inline
int		_json_is_type_str(
	const int _type
)
{
	return (_type & json_tok_str);
}

inline
int		_json_is_type_bool(
	const int _type
)
{
	return (_type & json_tok_bool);
}

inline
int		_json_is_type_null(
	const int _type
)
{
	return (_type & json_tok_null);
}