// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "../standards/lib_config.h"
# include "./standards/lib_standards.h"

/* -----| Internals |----- */
// # include "dotenv/types.h"

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
 * @brief	load the file .env into the programm env
 */
int	dotenv_load(void);

/**
 * @brief	load the file at `path` into the env
 */
int	dotenv_load_file(
	const char *const _path
);