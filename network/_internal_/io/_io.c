// Header

/* ----| Headers    |----- */
	/* Standard */
#include <unistd.h>
	/* Internal */
#include "_io.h"
	/* External */
#include "lib.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

int	_net_conn_init(
	t_net_conn *_p,
	char *const restrict _in_buff,
	char *const restrict _out_buff,
	const unsigned int _in_len,
	const unsigned int _out_len
)
{
	memset(_p, 0, sizeof(*_p));

	*_p = (t_net_conn){
		.fd = -1,
		.in_buff = _in_buff,
		.in_len = _in_len,
		.out_buff = _out_buff,
		.out_len = _out_len,
	};
	return (lib_network_error_none);
}

static int	_net_try_connect(
	t_net_conn *const restrict _p,
	const char *const restrict _ip,
	const int _port
)
{
	struct sockaddr_in	_addr;
	int					_fd;

	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (unlikely(_fd < 0))
		return (-1);

	memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(_port);
	inet_pton(AF_INET, _ip, &_addr.sin_addr);

	if (unlikely(connect(_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0))
		return (close(_fd), lib_network_error_connection_failed);
	_p->fd = _fd;

	return (lib_network_error_none);
}

int	_net_fast_connect(
	t_net_conn *const restrict _p,
	const char *const restrict _ip,
	const int _port
)
{
	return (_net_try_connect(_p, _ip, _port));
}

int	_net_connect(
	t_net_conn *const restrict _p,
	const char *const restrict _ip,
	const int _port
)
{
	register int	_i = 0;

	logs(log_debug, 0, "trying connection to %s:%d", _ip, _port);
	while (_i < NET_MAX_RETRY)
	{
		if (!_net_try_connect(_p, _ip, _port))
		{
			logs(log_debug, 1, "connected to %s:%d", _ip, _port);
			return (lib_network_error_none);
		}
		// logs(log_debug, 1, "connection failed to %s:%d. retry in %dms", _ip, _>port, delay);
		// usleep(_p->delay * 1000 * (_i + 1) / 4);
		_i++;
	}
	logs_perror(log_warning, 0, "NET_MAX_RETRY(%d) reach giving up on connecting %d:%d", NET_MAX_RETRY, _ip, _port);
	return (-lib_network_error_timeout);
}

ssize_t	_net_send(
	t_net_conn *const restrict _p
)
{
	unsigned int	_sent = 0;
	ssize_t			result = 0;

	while (result < _p->out_len)
	{
		_sent = send(_p->fd, _p->out_buff + result, _p->out_len - result, 0);
		if (unlikely(_sent < 0))
		{
			logs_perror(log_warning, 0, "send failed");
			_net_close(_p);
			_p->fd = -1;
			return (-(lib_network_error_send_failed));
		}
		result += _sent;
	}
	return ((ssize_t)result);
}

ssize_t	_net_recv(
	t_net_conn *const restrict _p
)
{
	size_t	_recvd = 0;
	ssize_t	result = 0;

	while (result < _p->in_len)
	{
		_recvd = recv(_p->fd, _p->in_buff + result, _p->in_len - result, 0);
		if (_recvd <= 0)
		{
			logs_perror(log_warning, 0, "send failed");
			_net_close(_p);
			_p->fd = -1;
			return (-(lib_network_error_recv_failed));
		}
		result += _recvd;
	}
	return ((ssize_t)result);
}

void	_net_close(
	t_net_conn *const restrict _conn
)
{
	close(_conn->fd);
}
