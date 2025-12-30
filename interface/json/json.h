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
 * @brief	do json_open() and json_parse() in one call
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
 */
JSON	*json_new(void);

/**
 * @brief	unload and free a JSON struct
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
			);

/**
 * TODO
 */
void	*json_fget(
			JSON *_json,
			const char *const restrict _field,
			...
			);

/* *************** */
/*     Writing     */
/* *************** */

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
			);

/**
 * TODO
 */
int		json_fset(
			JSON *_json,
			const char *const restrict _field,
			const void *_value,
			...
			);

/**
 * TODO
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
int		json_fset(
			JSON *_json,
			const char *const restrict _field,
			const void *_format,
			...
			);

/**
 * @brief	unset the field of the json element based on _field
 * 
 * @param	_json	the json element
 * @param	_field	the field to be set
 * @param	_free	bool value to say if the function need to free the removed field
 * 
 * @return	the errnum of the action
 * 
 * @example json_set(json, "data.key.git.user1");
*/
int		json_unset(
			JSON *_json,
			const char *const restrict _field
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