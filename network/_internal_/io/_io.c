// Header

/* ----| Headers    |----- */
	/* Standard */
#include <unistd.h>
	/* Internal */
#include "_io.h"
	/* External */
#include "log.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

int	_net_conn_init(
	t_net_conn *_p,
	const char *const restrict _ip,
	const int _port,
	const int _retry_delay_ms
)
{
	memset(_p, 0, sizeof(_p));
	strncpy(_p->ip, _ip, 15);
	_p->port = _port;
	_p->delay = _retry_delay_ms;
	_p->fd = -1;
	return (lib_network_error_none);
}

static int	_net_try_connect(
	t_net_conn *_p
)
{
	struct sockaddr_in	_addr;
	int					_fd;

	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (unlikely(_fd < 0))
		return (-1);

	memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(_p->port);
	inet_pton(AF_INET, _p->ip, &_addr.sin_addr);

	if (unlikely(connect(_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0))
		return (close(_fd), lib_network_error_connection_failed);
	_p->fd = _fd;

	return (lib_network_error_none);
}

int	_net_fast_connect(
	t_net_conn *_p
)
{
	return (_net_try_connect(_p));
}

int	_net_connect(
	t_net_conn *_p
)
{
	register int	_i = 0;

	logs(log_debug, 0, "trying connection to %s:%d\n", _p->ip, _p->port);
	while (_i < NET_MAX_RETRY)
	{
		if (_net_try_connect(_p))
		{
			logs(log_debug, 1, "connected to %s:%d\n", _p->ip, _p->port);
			return (lib_network_error_none);
		}
		logs(log_debug, 1, "connection failed to %s:%d. retry in %dms\n", _p->ip, _p->port, _p->delay);
		usleep(_p->delay * 1000 * (_i + 1) / 4);
		_i++;
	}
	logs_perror(log_warning, 0, "NET_MAX_RETRY(%d) reach giving up on connecting %d:%d", NET_MAX_RETRY, _p->ip, _p->port);
}

ssize_t	_net_send(
	t_net_conn *const restrict _p,
	const void *const restrict _buff,
	const size_t _size
)
{
	size_t	_sent = 0;
	ssize_t	result = -1;

	if (unlikely(_p->fd < 0))
		_net_connect(_p);

	while (_sent < _size)
	{
		result = send(_p->fd, (char *)_buff + _sent, _size - _sent, 0);
		if (unlikely(result < 0))
		{
			logs_perror(log_warning, 0, "send failed, reconnecting");
			_net_close(_p->fd);
			_p->fd = -1;
			_net_connect(_p);
			return -(lib_network_error_send_failed);
		}
		_sent += result;
	}
	return ((ssize_t)_sent);
}

ssize_t	_net_recv(
	t_net_conn *const restrict _p,
	void *const restrict _buff,
	const size_t _size
)
{
	size_t	_recvd = 0;
	ssize_t	result;

	while (_recvd < _size)
	{
		result = recv(_p->fd, (char *)_buff + _recvd, _size - _recvd, 0);
		if (result <= 0)
		{
			logs_perror(log_warning, 0, "send failed, reconnecting");
			_net_close(_p->fd);
			_p->fd = -1;
			_net_connect_persistant(_p);
			return (lib_network_error_recv_failed);
		}
			return (result);
		_recvd += result;
	}
	return ((ssize_t)_recvd);
}

void	_net_close(
	t_net_conn *_conn
)
{
	if (likely(_conn->fd > 0))
		close(_conn->fd);
}
