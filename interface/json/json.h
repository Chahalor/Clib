// Header

/**
 * TODO: add a format version to all function that take a field as args
 */

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stdio.h>
#include <stdarg.h>

/* -----| Globals   |----- */
# include "lib_config.h"
# include "lib_standards.h"

/* -----| Internals |----- */
# include "types.h"

/* -----| Modules   |----- */
#include "memory.h"
#include "_internal_/_json.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

#ifdef _JSON_FOEACH_ARRAY
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
 * if an "=<any format>" is added at the end of `field` the type of `value` will be taken from here.
 * Other with the last argument of the function will have to be the type of args has to be added
 * 
 * @param	json	the json element
 * @param	field	the field to be set
 * @param	value	the value to be writen
 * 
 * @return	the errnum of the action
 * 
 * @example json_set(json, "data.key.git.user1", var, json_str);
 * @example json_set(json, "data.key.git.user1=%s", var);
 * 
 * @version	2.0.0
*/
int		json_set(
			JSON *_json,
			const char *const restrict _field,
			const void *_value,
			...
			);

/**
 * @brief	unset the field of the json element based on _field
 * 
 * handle va args for field
 * 
 * @param	_json	the json element
 * @param	_field	the field to be set
 * @param	_free	bool value to say if the function need to free the removed field
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

/** */
int		json_remove(
			JSON *_json,
			const char *const restrict _field
			);


/* *************** */
/*     Saving      */
/* *************** */

int		json_dump(
			JSON *_json,
			FILE *_file,
			const int pretty
			);

char	*json_stringify(
			JSON *_json,
			const int pretty
			);

/* *************** */
/*     checks      */
/* *************** */

/** */
int		json_assert_type(
			const JSON *const _node,
			const int type
			);

/** */
int		json_get_type(
			const JSON *const _node
			);