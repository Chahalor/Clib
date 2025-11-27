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

int		net_server_start(
	t_net_server *const restrict _server,
	const int _port
)
{
	if (unlikely(!_server))
		return (-error_invalid_arg);
	else
		return (_net_server_start(_server, _port));
}

int		net_sever_accept(
	t_net_server *const _server
)
{
	if (unlikely(!_server))
		return (-error_invalid_arg);
	else
		return (_net_server_accept(_server));
}

int		net_server_close(
	t_net_server *const _server
)
{
	if (unlikely(!_server))
		return (-error_invalid_arg);
	else
		return (_net_server_close(_server));
}

#pragma region Client

int		net_connect(
	t_net_conn	*_conn
)
{
	if (unlikely(!_conn))
		return (error_invalid_arg);
	else
		return (_net_connect(_conn));
}

#pragma region IO

ssize_t	net_send(
	t_net_conn *_conn,
	const void *const restrict _buff,
	const size_t _size
)
{
	if (unlikely(!_conn || _conn->fd < 0 || !_buff))
		return (-error_invalid_arg);
	else
		return (_net_send(_conn, _buff, _size));
}

ssize_t	net_recv(
	t_net_conn *_conn,
	void *const restrict _buff,
	const size_t _size
)
{
	if (unlikely(!_conn || _conn->fd < 0 || !_buff))
		return (-error_invalid_arg);
	else
		return (_net_recv(_conn, _buff, _size));
}


void	net_close(
	t_net_conn *_conn
)
{
	if (unlikely(!_conn || _conn->fd < 0))
		return ;
	else
		return (_net_close(_conn));
}
