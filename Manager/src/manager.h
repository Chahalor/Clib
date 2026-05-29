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
# include "interface/args.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

#define	STR_HELPER(x)	#x
#define	STR(x) 			STR_HELPER(x)

#define	VERSION_MAJOR	1
#define	VERSION_MINOR	0
#define	VERSION_PATCH	0

#define	VERSION			STR(VERSION_MAJOR) "." STR(VERSION_MINOR) "." STR(VERSION_PATCH)

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

int	setup_setup(
	Config *const			config,
	t_args_output *const	output
);

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

int	config_load(
	Config *const		config,
	const char *const	path
);

int	modules_load(
	Config *const		config,
	const char *const	path
);

int	init_cache(
	const Config *const config
);

int	init_all(
	Config *const config
);
