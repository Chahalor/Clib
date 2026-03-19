// Header

/* ----| Headers    |----- */
	/* Standard */
		//...
	/* Internal */
#include "network.h"
#include "_internal_/_network.h"
	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

#pragma region Server

int		net_init_conn(
	t_net_conn *_p,
	char *const restrict _in_buff,
	char *const restrict _out_buff,
	const unsigned int _in_len,
	const unsigned int _out_len
)
{
	if (unlikely(!_p || !_in_buff || !_out_buff))
		return (-(error_invalid_arg));
	else
		return (_net_conn_init(_p, _in_buff, _out_buff, _in_len, _out_len));
}

int		net_server_start(
	t_net_conn *const restrict _server,
	const int _port
)
{
	if (unlikely(!_server || _port < 0))
		return (-(error_invalid_arg));
	else
		return (_net_server_start(_server, _port));
}

int		net_server_accept(
	t_net_conn *const restrict _server
)
{
	if (unlikely(!_server))
		return (-(error_invalid_arg));
	else
		return (_net_server_accept(_server));
}

int		net_server_close(
	t_net_conn *const restrict _server
)
{
	if (unlikely(!_server))
		return (-(error_invalid_arg));
	else
		return (_net_server_close(_server));
}

#pragma region Client

int		net_connect(
	t_net_conn *const restrict _p,
	const char *const restrict _ip,
	const int _port
)
{
	if (unlikely(!_p || !_ip || _port < 0))
		return (-(error_invalid_arg));
	else
		return (_net_connect(_p, _ip, _port));
}

#pragma region IO

ssize_t	net_send(
	t_net_conn *const restrict _p
)
{
	if (unlikely(!_p || _p->fd < 0))
		return (-(error_invalid_arg));
	else
		return (_net_send(_p));
}

ssize_t	net_recv(
	t_net_conn *const restrict _p
)
{
	if (unlikely(!_p || _p->fd < 0))
		return (-(error_invalid_arg));
	else
		return (_net_recv(_p));
}


void	net_close(
	t_net_conn *const restrict _conn
)
{
	if (unlikely(!_conn || _conn->fd < 0))
		return ;
	else
		return (_net_close(_conn));
}
