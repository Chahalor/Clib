// Header

/* ----| Headers    |----- */
	/* Standard */
		//...
	/* Internal */
#include "_server.h"
	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

int		_net_server_start(
	t_net_server *const restrict _server,
	const int _port
)
{
	int					_fd = -1;
	int					_opt = 1;
	struct sockaddr_in	_addr = {0};

	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (unlikely(_fd < 0))
		return (lib_network_error_socket_failed);
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &_opt, sizeof(_opt));

	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(_port);

	if (unlikely(bind(_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0))
	{
		close(_fd);
		return (lib_network_error_bind_failed);
	}
	else if (unlikely(listen(_fd, NET_MAX_LISTEN) < 0))
	{
		close(_fd);
		return (lib_network_error_bind_failed);
	}

	_server->fd = _fd;
	_server->port = _port;
	return (lib_network_error_none);
}

int		_net_server_accept(
	t_net_server *const _server
)
{
	return (accept(_server->fd, NULL, NULL));
}

int		_net_server_close(
	t_net_server *const _server
)
{
	if (likely(_server->fd > 0))
		return (close(_server->fd));
	_server->fd = -1;
	return (error_none);
}
