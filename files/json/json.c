// Header

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>

	/* Internal */
#include "_internal_/_json.h"
#include "json.h"
	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

#pragma region Loading

JSON	*json_load(
	const char *const restrict _filename
)
{
	JSON	*result = NULL;

	if (unlikely(!_filename))
		return (NULL);
	else
		return (_json_parser_file(&result, _filename), result);
}

JSON	*json_load_str(
	const char *const restrict _str,
	...
)
{
	JSON	*result = NULL;
	va_list	_args;

	if (unlikely(!_str))
		return (NULL);

	va_start(_args, _str);
	if (unlikely(_json_parser_va_string(&result, _str, &_args) != error_none))
	{
		_json_free_all(result);
		result = NULL;
	}

	va_end(_args);
	return (result);
}

JSON	*json_new(void)
{
	return (_json_new_content(NULL, json_tok_obj, NULL));
}

int		json_unload(
	JSON *_json
)
{
	if (unlikely(!_json))
		return (-(error_invalid_arg));
	else
		return (_json_free_all(_json));
}


#pragma region Reading

void	*json_get(
	JSON *_json,
	const char *const restrict _format,
	...
)
{
	va_list	_args;
	void	*result = NULL;

	if (unlikely(!_json || !_format))
		return (NULL);

	va_start(_args, _format);

	_json_get(_json, _format, &_args, &result);

	va_end(_args);
	return (result);
}

/**
 * @brief	return the size of all element of the json, it is the total number of byte allocated by the json
 * 
 * @param	_json	The targeted json variable
 * 
 * @return	the total size (bytes) of the object
*/
size_t	json_sizeof(
	const JSON *const restrict _json
)
{
	if (unlikely(!_json))
		return (0);
	else
		return (_json_node_size(_json));
}

/**
 * @brief	return the number of element of the json
 * 
 * @param	_json	the targeted json
 * 
 * @return	the numer of element of the json
 */
size_t	json_len(
	const JSON *const restrict _json
)
{
	if (unlikely(!_json))
		return (0);
	else
		return (_json_access_len(_json));
}


#pragma region Writing

int		json_set(
	JSON *_json,
	const char *const restrict _field,
	const void *_value,
	const unsigned int _type,
	...
)
{
	va_list	_args;
	int		result = error_none;

	if (unlikely(!_json || !_field || _type > json_tok_obj))
		return (error_invalid_arg);

	va_start(_args, _type);

	result = _json_set_va_args(&_json, _field, _value, _type, &_args);

	va_end(_args);
	return (result);
}

int	json_set_from_array(
	JSON *_json,
	const char *const restrict _field,
	void *const _array,
	const size_t _length,
	const int _type,
	...
)
{
	va_list	_args;
	int		result = error_none;

	va_start(_args, _type);
	result = _json_set_array_va_list(&_json, _field, _array, _length, _type, &_args);

	va_end(_args);
	return (result);
}

int	json_array_append(
	JSON *const _json,
	void *const _value,
	const int _type
)
{
	int		result = error_none;

	result = _json_append_array(_json, _value, _type);

	return (result);
}

int	json_array_pop(
	JSON *const _json,
	const size_t _index
)
{
	return (_json_pop_array(_json, _index));
}

int		json_unset(
	JSON *_json,
	const char *restrict _field,
	...
)
{
	va_list	_args;
	int		result = error_none;

	if (unlikely(!_json || !_field))
		return (error_invalid_arg);
	
	va_start(_args, _field);
	result = _json_unset(&_json, _field, false, &_args);

	va_end(_args);
	return (result);
}

int	json_remove(
	JSON *_json,
	const char *restrict _field,
	...
)
{
	va_list	_args;
	int		result = error_none;

	if (unlikely(!_json || !_field))
		return (error_invalid_arg);

	va_start(_args, _field);
	result = _json_unset(&_json, _field, true, &_args);

	va_end(_args);
	return (result);
}

#pragma region Saving


int		json_dump(
	JSON *const restrict _json,
	FILE *const _file,
	const int _pretty
)
{
	if (unlikely(!_json || !_file))
		return (error_invalid_arg);
	else
		return (_json_dump(_json, _file, _pretty));
}

char	*json_stringify(
	JSON *_json,
	const int _pretty
)
{
	int		_errnum = error_none;
	char	*result = NULL;

	if (unlikely(!_json))
		return (NULL);

	result = _json_stringify(_json, _pretty, &_errnum);
	return (result);
}


#pragma region Checks

/** */
int		json_assert_type(
	const JSON *const _node,
	const int _type
)
{
	return (_node ?
				_node->type == _type :
				0
			);
}

/** */
int		json_get_type(
	const JSON *const _node
)
{
	return (_node ?
				_node->type :
				0
			);
}
