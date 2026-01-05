// Header

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
// # include "types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/**
 * @brief	return a value between val1 and val2
 * 
 * @param	val1	first value
 * @param	val2	second value
 * 
 * @return a random value
 * 
 * @version 0.0.1
 */
int	randint(
		const int _val1,
		const int _val2
		);

/**
 * @brief	retrun a malloc'd array of int size long filled with random values between val1 and val2
 * 
 * @param	size	the size of the array
 * @param	val1	first value for the range
 * @param	val2	seconde vale for the range
 * 
 * @return	the dynamicly NULL terminated array or NULL
 * @retval		ptr: the array
 * @retval		NULL: some errors append
 * 
 * @version	0.0.1
*/
int	*rand_range(
		const unsigned int _size,
		const int _val1,
		const int _val2
		);