// Header

/* ----| Headers    |----- */
	/* Standard */
		//...
	/* Internal */
#include "_io.h"
	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

ssize_t	net_send(
	const int _fd,
	const void *const restrict _buff,
	const size_t _size
)
{
	size_t	_sent = 0;
	ssize_t	result = -1;

	while (_sent < _size)
	{
		result = send(_fd, (char *)_buff + _sent, _size - _sent, 0);
		if (unlikely(result < 0))
			return (result);
		_sent += result;
	}
	return ((ssize_t)_sent);
}

ssize_t	net_recv(
	const int _fd,
	void *const restrict _buff,
	const size_t _size
)
{
	size_t	_recvd = 0;
	ssize_t	result;

	while (_recvd < _size)
	{
		result = recv(_fd, (char *)_buff + _recvd, _size - _recvd, 0);
		if (result <= 0)
			return (result);
		_recvd += result;
	}
	return ((ssize_t)_recvd);
}

void	net_close(
	const int _fd
)
{
	if (likely(_fd > 0))
		close(_fd);
}
