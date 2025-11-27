// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <sys/types.h>

/* -----| Globals   |----- */
# include "config.h"
# include "standards.h"

/* -----| Internals |----- */
# include "types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

int		net_server_start(
			t_net_server *const restrict _server,
			const int _port
			);

int		net_sever_accept(
			t_net_server *const _server
			);

int		net_server_close(
			t_net_server *const _server
			);

int		net_connect(
			t_net_conn *_conn
			);

ssize_t	net_send(
			t_net_conn *_conn,
			const void *const restrict _buff,
			const size_t _size
			);

ssize_t	net_recv(
			t_net_conn *_conn,
			void *const restrict _buff,
			const size_t _size
			);

void	net_close(
			t_net_conn *_conn
			);
