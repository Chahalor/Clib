// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
#include "lib_standards.h"

/* -----| Internals |----- */
	//...

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */

typedef struct s_net_conn	t_net_conn;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

enum e_lib_network_error
{
	lib_network_error_none = error_none,
	lib_network_error_socket_failed,
	lib_network_error_bind_failed,
	lib_network_error_listen_failed,
	lib_network_error_connection_failed,
	lib_network_error_send_failed,
	lib_network_error_recv_failed,
	lib_network_error_timeout,
};

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

struct s_net_conn
{
	char			*in_buff;	// buffer for incoming recv()
	char			*out_buff;	// buffer for send() & Co

	unsigned int	in_len;		// length of the in_buff
	unsigned int	out_len;	// length of the out_buff

	int				fd;			// for or the connection
};

