// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
#include "standards.h"

/* -----| Internals |----- */
	//...

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */

typedef struct s_log_file	t_log_file;
typedef struct s_log_init	t_log_init;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

enum e_log_level
{
	log_error,
	log_warning,
	log_info,
	log_debug,
	log_other
};

enum e_log_close_action
{
	log_close_all,
	log_close_one,
};

enum e_log_error
{
	log_error_none = error_none,
	log_error_invalid_logfile,
	log_error_write_impossible,
};

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

struct s_log_file
{
	char	*filename;	// file for the log
	int		mode;		// open mode 
	int		perms;		// perms in case of creation
};

struct s_log_init
{
	t_log_file	error;			//information about the error log stream
	t_log_file	warning;		//information about the warning log stream
	t_log_file	info;			//information about the info log stream
	t_log_file	debug;			//information about the deug log stream
	t_log_file	other;			//information about the other log stream
	int			display_limit;	// limits where all lower level are send to the STDERR
};
