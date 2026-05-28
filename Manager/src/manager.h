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
// # include "config.h"
// # include "standards.h"

/* -----| Internals |----- */
# include "types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

#define	ALLOC_SIZE			256

#define	HOME				"~"
#define	DEFAULT_CACHE_DIR	HOME"/.clib"
#define	DEFAULT_CONFIG_FILE	".clib"
#define	DEFAULT_URL_GIT		""

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

int	array_alloc(
	t_array *const	array,
	uint32_t		capacity
);

void		array_free(
				t_array *const	array,
				const int		all
			);

int	array_append(
	t_array *const	array,
	void *const		data
);

t_module	*module_new(void);

/** */
int	setup(
	Config *const config
);

/** */
int	cmake_write(
	const Config *const	config
);

int	execute(
	const char *const	command,
	char *const *		argv
);
