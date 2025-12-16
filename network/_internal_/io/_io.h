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
			char *const restrict _in_buff,
			char *const restrict _out_buff,
			const unsigned int _in_len,
			const unsigned int _out_len
			);

int		_net_fast_connect(
			t_net_conn *const restrict _p,
			const char *const restrict _ip,
			const int _port
			);

int		_net_connect(
			t_net_conn *const restrict _p,
			const char *const restrict _ip,
			const int _port
			);

ssize_t	_net_send(
			t_net_conn *const restrict _p
			);

ssize_t	_net_recv(
			t_net_conn *const restrict _p
			);

void	_net_close(
			t_net_conn *const restrict _conn
			);