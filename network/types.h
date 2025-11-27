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

typedef struct s_net_connection	t_net_connection;
typedef t_net_connection		t_net_conn;
typedef struct s_net_server		t_net_server;

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
};

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

struct s_net_connection
{
	char	ip[16];
	int		port;
	int		fd;
	int		delay;	// delay beteewn connection retry in case of disconnect
};

struct s_net_server
{
	int	fd;
	int	port;
};

// struct s_net_packet
// {
	
// };
