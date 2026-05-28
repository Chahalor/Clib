/**
 * 
*/
# pragma once


/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stdint.h>

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

typedef struct s_array	t_array;
typedef struct s_module	t_module;
typedef struct s_config	Config;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

enum e_subs
{
	SETUP,
	UPDATE,
	EXPORT,
	UNKNOW,
};

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

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

struct s_config
{
	struct s_conf_file	conf;
	char				*config_file;
	char				*dest;
	enum e_subs			sub;
};
