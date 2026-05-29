/**
 * 
*/
# pragma once


/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stdint.h>
#include <assert.h>

/* -----| Globals   |----- */
	//...

/* -----| Internals |----- */
	//...

/* -----| Modules   |----- */
#include "files/toml.h"

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */
/* -----| Typename  |----- */
	//...

/* -----| Enums     |----- */
	//...

/* -----| Unions    |----- */
	//...

/* -----| Structs   |----- */

typedef struct s_array		t_array;
typedef struct s_module		t_module;
typedef struct s_sub_list	t_sub_list;
typedef struct s_config		Config;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

enum e_subs
{
	INIT,
	SETUP,
	UPDATE,
	LIST,
	HELP,
	UNKNOW,
};

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */

union u_version
{
	struct
	{
		int	major;
		int	minor;
		int	patch;
	};

	int	array[3];
};

_Static_assert(sizeof(union u_version) == sizeof(int) * 3,
	"unstable u_version union full size is not 3 int");

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

typedef struct s_array
{
	void		**data;
	uint32_t	length;
	uint32_t	capacity;
}	t_array;

struct s_module
{
	t_array	dependencies;
	t_array	defines;
	t_array	tags;
	t_array	controls;
	char	*name;
	char	*path;
	char	*public_header;
	char	*private_header;
};

struct s_conf_file
{
	t_array	modules;
	TOML	*toml;
	char	*name;
	char	*version;
	char	*remote_url;	// url of the remote git
};

struct s_cli
{
	char	verbose;
	char	help;
	char	init;
};

struct s_constants
{
	union u_version	version;
	char			*path_cache_dir;
	char			*path_config_file;
	char			*path_modules_file;
	char			*url_git;
};

struct s_sub_list
{
	struct
	{
		char	deps;		// show all dependencies of all modules
		char	version;	// show the version of each module
		char	stats;		// show stats about the module (nb file/size/...)
		char	installed;	// show which modules are installed in local
		char	config;		// display information about the config/cache
	}	options;
	char	*deps_dir;		// dir targeted by the --deps=<name> option
};

struct s_config
{
	struct s_conf_file	lib;
	struct s_cli		cli;
	struct s_constants	consts;
	struct
	{
		enum e_subs	name;
		t_sub_list	list;
	}	sub;
	char				*config_file;
	char				*dest;
	char				**allowed;
	uint32_t			nb_allowed;
};
