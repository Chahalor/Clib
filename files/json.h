/**
 * @file json.h
 * @brief API public for the json module
 *
 * This module manage all interaction with json file/struct.
 * From reading thems, modifing thems and writing thems.
 *
 * @ingroup json
 */

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stdio.h>
#include <stdarg.h>

/* -----| Globals   |----- */
# include "../standards/lib_config.h"
# include "../standards/lib_standards.h"

/* -----| Internals |----- */
#include "json/_internal_/_json.h"
# include "json/types.h"

/* -----| Modules   |----- */
#include "../memory.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

#ifdef _JSON_FOEACH_ARRAY
/**
 * @def	json_foreach
 * 
 * @brief	do a foreach loop with all element of a json node
 * 
 * should be use like a for in a for loop
 * 
 * @param	var		the name of the var ceated to acces json elements
 * @param	node	the node to make the loop on
 * 
 * @version	1.1.0
 */
# define json_foreach(var, node) _JSON_FOEACH_ARRAY(var, node)
#endif

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/* *************** */
/*     loading     */
/* *************** */

/**
 * @brief	load a json from a file
 * 
 * @param	filename	the file to load the json from
 * 
 * @return	the loaded json or NULL in case of error
 * 
 * @example	JSON	*var = json_load("file.json");
 * 
 * @version	1.0.0
*/
JSON	*json_load(
			const char *const restrict _filename
			);

/**
 * @brief	load the json variable from the given format
 * 
 * use the same format than printf
 * 
 * @param	format	the format to be loaded
 * 
 * @return	the JSON variable or NULL in case of error
 * 
 * @example	JSON	*var = json_load_str("{\"bob\": 10}");
 * @example	JSON	*var = json_load_str("{\"bob\": %d}", 10);
 * 
 * @version	2.0.0
 */
JSON	*json_load_str(
			const char *const restrict _format,
			...
			);

/**
 * @brief	alocate an empty JSON object
 * 
 * @return	the empty json object or NULL if the alloc failed
 * 
 * @example	JSON	*var = json_new();
 * 
 * @version	1.0.0
 */
JSON	*json_new(void);

/**
 * @brief	unload and free a JSON struct
 * 
 * @param	json	the json to be freed
 * 
 * @return	error_none or the errnum of the error
 * 
 * @version	1.0.0
 */
int		json_unload(
			JSON *_json
			);

/* *************** */
/*     Reading     */
/* *************** */

/**
 * @brief	get the json node of the current path starting with the passed node
 * 
 * @param	json	the current json node
 * @param	format	the field to be accessed
 * 
 * @return	the wanted node or NULL if not found
 * 
 * @example char	*var = json_get(json, "data.key.git")
 * @example char	*var = json_get(json, "data/key/git")
 * @example char	*var = json_get(json, "data/%s/git", "key")
 * 
 * @version 2.0.0
 */
void	*json_get(
			JSON *_json,
			const char *const restrict _field,
			...
			);

/**
 * @brief	return the size of all element of the json, it is the total number of byte allocated by the json
 * 
 * @param	json	The targeted json variable
 * 
 * @return	the total size (bytes) of the object
 * 
 * @version	1.0.0
*/
size_t	json_sizeof(
			const JSON *const restrict _json
			);

/**
 * @brief	return the number of element of the json
 * 
 * @param	json	the targeted json
 * 
 * @return	the numer of element of the json
 * 
 * @version	1.0.0
 */
size_t	json_len(
			const JSON *const restrict _json
			);

/* *************** */
/*     Writing     */
/* *************** */

/**
 * @brief	set the field of the json element with _value
 * 
 * @param	json	the json element
 * @param	field	the field to be set
 * @param	value	the value to be writen
 * 
 * @return	the errnum of the action
 * 
 * @example json_set(json, "data.key.git.user1", var, json_str);
 * @example json_set(json, "data.key.git.%s", var, json_str, "user1");
 * @example json_set(json, "data.key.git.user1", "bob is %s", json_str, "bob");
 * 
 * @version	2.0.0
*/
int		json_set(
			JSON *_json,
			const char *const restrict _field,
			const void *_value,
			const unsigned int _type,
			...
			);

/**
 * @brief	set the array field of the json element from a C array
 * 
 * @param	json	the json element
 * @param	field	the field to be set
 * @param	array	the source array
 * @param	len		the number of elements in array
 * @param	type	the json element type for each entry
 * 
 * @return	the errnum of the action
 * 
 * @example json_set_from_array(json, "data.list", (int[]){1,2,3}, 3, json_tok_nbr);
 * @example json_set_from_array(json, "data.names", (char *[]){"bob","alice"}, 2, json_tok_str);
 * 
 * @version	2.1.0
*/
int		json_set_from_array(
			JSON *_json,
			const char *const restrict _field,
			void *const _array,
			const size_t _length,
			const int _type,
			...
			);

/**
 * @brief	add a new value at the end of the array.
 * `json` should be the node of the array.
 * `json_tok_obj` and `json`tok`array` are not suported
 */
int		json_array_append(
			JSON *const _json,
			void *const _value,
			const int _type
			);

/**
 * @brief	remove the value at the `index`
*/
int		json_array_pop(
			JSON *const _json,
			const size_t _index
			);

/**
 * @brief	unset the field of the json element based on _field
 * 
 * handle va args for field
 * this function will **not** free what it unset
 * 
 * @param	json	the json element
 * @param	field	the field to be set
 * 
 * @return	the errnum of the action
 * 
 * @example json_set(json, "data.key.git.user1");
 * @example json_set(json, "data.key.%s.user1", "git");
 * 
 * @version	2.0.0
*/
int		json_unset(
			JSON *_json,
			const char *const restrict _field,
			...
			);

/**
 * @brief	remove the field of the json element based on _field
 * 
 * handle va args for field
 * this function will free what it remove
 * 
 * @param	json	the json element
 * @param	field	the field to be set
 * 
 * @return	the errnum of the action
 * 
 * @example json_remove(json, "data.key.git.user1");
 * @example json_remove(json, "data.key.%s.user1", "git");
 * 
 * @version	2.0.0
*/
int		json_remove(
			JSON *_json,
			const char *const restrict _field,
			...
			);


/* *************** */
/*     Saving      */
/* *************** */

/**
 * @brief	dump the content of the `json` to `file`
 * 
 * @param	json	the json struct to be dump
 * @param	file	the FILE where to dump
 * @param	pretty	number of space added to be humanly readable
 * 
 * @return	error_none or the error code of the failure
 * @retval		`error_invalid_arg`: invalid args
 * @retval		`json_error_io`: invalid/unreadable file
 * @retval		`error_alloc_fail`: allocation failure
 * 
 * @example	json_dump(json, stdout, 4);	// will print the json in the stdout with 4 space has indent
 * @example	json_dump(json, stdout, 0);	// will print the json in the stdout with no space has indent
 * @example	json_dump(json, NULL, 0);	// will failed
 * 
 * @note	use `json_stringify` logic to create the string
 * 
 * @version	1.0.1
 */
int		json_dump(
			JSON *const restrict _json,
			FILE *const _file,
			const int _pretty
			);

/**
 * @brief	return a string of `json`
 * 
 * @param	json	the json struct to be stringify
 * @param	pretty	number of space added to be humanly readable
 * 
 * @return	error_none or the error code of the failure
 * @retval		`error_invalid_arg`: invalid args
 * @retval		`error_alloc_fail`: allocation failure
 * 
 * @example	char	*str = json_stringify(json , 4);
 * @example	char	*str = json_stringify(json , 0);
 * 
 * @version	1.0.1
 */
char	*json_stringify(
			JSON *_json,
			const int pretty
			);

/* *************** */
/*     checks      */
/* *************** */

/**
 * @brief	assert the type of the current node
 * 
 * @param	node	the node to be tested
 * @param	type	the type to be compared
 * 
 * @return	true or false depending of the result
 * 
 * @version	1.0.0
*/
int		json_assert_type(
			const JSON *const _node,
			const int type
			);

/**
 * @brief	return the type of the current node
 * 
 * @param	node	the node examined
 * 
 * @return	the node type
 * 
 * @version	1.0.0
*/
int		json_get_type(
			const JSON *const _node
			);
