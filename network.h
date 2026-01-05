// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <sys/types.h>

/* -----| Globals   |----- */
# include "lib_config.h"
# include "lib_standards.h"

/* -----| Internals |----- */
# include "network/types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

int		net_init_conn(
			t_net_conn *_p,
			char *const restrict _in_buff,
			char *const restrict _out_buff,
			const unsigned int _in_len,
			const unsigned int _out_len
			);

int		net_server_start(
			t_net_conn *const restrict _server,
			const int _port
			);

int		net_server_accept(
			t_net_conn *const restrict _server
			);

int		net_server_close(
			t_net_conn *const restrict _server
			);

int		net_connect(
			t_net_conn *const restrict _p,
			const char *const restrict _ip,
			const int _port
			);

ssize_t	net_send(
			t_net_conn *const restrict _p
			);

ssize_t	net_recv(
			t_net_conn *const restrict _p
			);

void	net_close(
			t_net_conn *const restrict _conn
			);
