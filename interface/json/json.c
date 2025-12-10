// Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdio.h>
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

// /**
//  * @brief	open the json file and do nothing other that setuping it
//  */
// JSON	*json_open(
// 	const char *const restrict _filename
// )
// {
// 	return (_json_new_root(_filename));
// }

// /**
//  * @brief	parse the JSON file if not already done
// */
// int		json_parse(
// 	JSON *_json
// )
// {
// 	if (unlikely(!_json))
// 		return (error_none);
// 	else
// 		return (_json_parser_file(_json));
// }

/**
 * @brief	do json_open() and json_parse() in one call
*/
JSON	*json_load(
	const char *const restrict _filename
)
{
	JSON	*result = NULL;

	if (unlikely(!_filename))
		return (NULL);

	// // result = _json_new_content(NULL, json_tok_obj, NULL);
	// if (unlikely(!result))
	// 	return (NULL);
	// else
	return (_json_parser_file(&result, _filename), result);
}

/**
 * @brief	load the json variable from a char *
 */
JSON	*json_load_str(
	const char *const restrict _str
)
{
	JSON	*result = NULL;

	if (unlikely(_json_parser_string(&result, (void *)_str, strlen(_str)) != error_none))
	{
		_json_free_all(result);
		return (NULL);
	}
	else
		return (result);
}

/**
 * @brief	alocate an empty JSON object
 */
JSON	*json_new(void)
{
	return (_json_new_content(NULL, json_tok_obj, NULL));
}

/**
 * @brief	unload and free a JSON struct
 */
int		json_unload(
	JSON *_json
)
{
	return (_json_free_all(_json));
}


#pragma region Reading


/**
 * @brief	get the json node of the current path starting with the passed node
 * 
 * @param	_json	the current json node
 * @param	_field	the field to be accessed
 * 
 * @return	the wanted node or NULL if not found
 * 
 * @example char	*var = json_get(json, "data.key.git")
 * @example char	*var = json_get(json, "data/key/git")
 */
void	*json_get(
	JSON *_json,
	const char *const restrict _field
)
{
	if (unlikely(!_json || !_field))
		return (NULL);
	else
		return (_json_access_field(_json, _field, -1));
}


#pragma region Writing


/**
 * @brief	set the field of the json element with _value
 * 
 * @param	_json	the json element
 * @param	_field	the field to be set
 * @param	_value	the value to be writen
 * @param	_type	the type of the value
 * 
 * @return	the errnum of the action
 * 
 * @example json_set(json, "data.key.git.user1", var, json_str);
*/
int		json_set(
	JSON *_json,
	const char *const restrict _field,
	const void *_value,
	const int _type
)
{
	if (unlikely(!_json || !_field))
		return (error_invalid_arg);
	else
		return (_json_set(&_json, _field, _value, _type));
}

/**
 * @brief	unset the field of the json element based on _field
 * 
 * @param	_json	the json element
 * @param	_field	the field to be set
 * 
 * @return	the errnum of the action
 * 
 * @example json_set(json, "data.key.git.user1");
*/
int		json_unset(
	JSON *_json,
	const char *const restrict _field
)
{
	if (unlikely(!_json || !_field))
		return (error_invalid_arg);
	else
		return (_json_unset(&_json, _field));
}


#pragma region Saving


int		json_dump(
	JSON *_json,
	FILE *_file,
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
	if (unlikely(!_json))
		return (NULL);
	else
		return (_json_stringify(_json, _pretty));
}


#pragma region Checks

/** */
int		json_assert_type(
	const JSON *const _node,
	const int _type
)
{
	return (_node->type == _type);
}

/** */
int		json_get_type(
	const JSON *const _node
)
{
	return (_node->type);
}
