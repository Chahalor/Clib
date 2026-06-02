/**
 * 
*/
#ifndef TYPES_TYPES_H
# define TYPES_TYPES_H

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
#include "lib_config.h"
#include "lib_standards.h"

/* -----| Internals |----- */
#include "setting.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */
/* -----| Typename  |----- */

typedef unsigned int	uint;
typedef unsigned char	uchar;

/* -----| Enums     |----- */
	//...

/* -----| Unions    |----- */
	//...

/* -----| Structs   |----- */

typedef struct s_array			t_array;
typedef struct s_hash_map_entry	t_hash_map_entry;
typedef struct s_hash_map		t_hash_map;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

enum e_types_errors
{
	TYPES_ERROR_NONE = error_none,
};

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

/**
 * 
*/
struct s_array
{
	void	*data;		// row data of any type
	size_t	length;		// number of element currently in this array
	size_t	capacity;	// number of elements of size `elt_size` allocated for this arra
	uint	elt_size;	// size of each element of the array
};

struct s_hash_map_entry
{
	char					*key;
	void					*value;
	struct s_hash_map_entry	*next;
	size_t					hash;
};

/**
 * 
*/
struct s_hash_map
{
	struct s_hash_map_entry	**map;
	uint					length;
	size_t					capacity;
};

#endif	// TYPES_TYPES_H