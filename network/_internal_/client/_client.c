// Header

/* ----| Headers    |----- */
	/* Standard */
		//...
	/* Internal */
#include "_client.h"
	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

int		_net_client_connect(
	const char *const _ip,
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
		return (close(_fd), -1);

	return (_fd);
}
