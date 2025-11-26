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
	const char *const _ip,
	const int _port
)
{
	if (unlikely(!_ip))
		return (-error_invalid_arg);
	else
		return (_net_client_connect(_ip, _port));
}

#pragma region IO

ssize_t	net_send(
	const int _fd,
	const void *const restrict _buff,
	const size_t _size
)
{
	if (unlikely(_fd < 0 || !_buff))
		return (-error_invalid_arg);
	else
		return (_net_io_send(_fd, _buff, _size));
}

ssize_t	net_recv(
	const int _fd,
	void *const restrict _buff,
	const size_t _size
)
{
	if (unlikely(_fd < 0 || !_buff))
		return (-error_invalid_arg);
	else
		return (_net_io_recv(_fd, _buff, _size));
}


void	net_close(
	const int _fd
)
{
	if (unlikely(_fd < 0))
		return ;
	else
		return ((void)_net_io_recv(_fd));
}
