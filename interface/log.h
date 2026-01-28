// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "../standards/lib_config.h"
# include "../standards/lib_standards.h"

/* -----| Internals |----- */
# include "log/types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

#define	LOG_NB_LEVEL	5	/* number of availaible logs level */

#define LOG_ERROR(_level, _depth, _message, ...) {logs_error(_level, _depth, "%s:%s "##_message, __func__, __LINE__, __VA_ARGS__)}
#define LOG_PERROR(_level, _depth, _message, ...) {logs_perror(_level, _depth, "%s:%s "##_message, __func__, __LINE__, __VA_ARGS__)}

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/** */
int	log_init(
		t_log_init *data
		);

int	logs(
		int level,
		int depth,
		const char *format,
		...
		);

int	logs_perror(
		int level,
		int depth,
		const char *format,
		...
		);

int	logs_error(	// TODO: should add my errno string message behind the message
		int level,
		int depth,
		const char *format,
		...
		);

int	log_close(
		const int access,
		const int level
		);