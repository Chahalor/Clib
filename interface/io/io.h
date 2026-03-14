// Big Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stddef.h>
#include <stdio.h>

/* -----| Globals   |----- */
# include "lib_config.h"
# include "lib_standards.h"

/* -----| Internals |----- */
# include "types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

# define FILE_BUFFER_READ_SIZE	1024
# define FILE_BUFFER_WRITE_SIZE	1024

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/**
 * @brief	return a newly allocated line from the file `_file`
*/
__attribute__((malloc))
char	*get_line(
	FILE *const _file
);