/**
 * 
*/

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
# include <sys/types.h>

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

#define	DEFAULT_CONFIG_DIR		"clib"
#define	DEFAULT_CONFIG_FILE		"config.toml"
#define	DEFAULT_CACHE_DIR		"clib"
#define	DEFAULT_MODULES_FILE	"Clib.toml"
#define	DEFAULT_URL_GIT			"git@github.com:Chahalor/Clib.git"

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

t_args_parser	*setup_args(void);

int	arsg_extract(
	t_args_output	*const	out,
	Config			*const	config
);

int	dir_rm(
	const char *const restrict	path
);

char	*path_join(
	const char *const	left,
	const char *const	right
);

int	mkdir_parent(
	const char *const	path
);

int	is_dir_empty(
	const char *const	path
);

int	cache_has_git_metadata(
	const char *const	path
);

int	mkdir_p(
	const char *	path,
	mode_t			mode
);

int	copy_dir(
	const char *const	src,
	const char *const	dst
);

int	count_dir_files(
	const char *const	path,
	const bool			recursive
);

size_t	dir_size(
	const char *const	path
);

int	copy_modules(
	const Config *const	config,
	const t_array *const	modules
);

int	setup_setup(
	Config *const			config,
	t_args_output *const	output
);

/** */
int	setup(
	Config *const config
);

/** */
int	list(
	const Config *const config
);

/** */
int	cmake_write(
	const Config *const	config,
	const t_array *const	modules
);

int	execute(
	const char *const	command,
	char *const *		argv
);

int	config_load(
	Config *const		config,
	const char *const	path,
	const int			should_init
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
