// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "lib_config.h"
# include "lib_standards.h"

/* -----| Internals |----- */
# include "../_network.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

int		_net_conn_init(
			t_net_conn *_p,
			const char *const restrict _ip,
			const int _port,
			const int _retry_delay_ms
			);

int		_net_fast_connect(
			t_net_conn *_p
			);

int		_net_connect(
			t_net_conn *_p
			);

ssize_t	_net_send(
			t_net_conn *const restrict _p,
			const void *const restrict _buff,
			const size_t _size
			);

ssize_t	_net_recv(
			t_net_conn *const restrict _p,
			void *const restrict _buff,
			const size_t _size
			);

void	_net_close(
			t_net_conn *_conn
			);