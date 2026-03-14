// Big Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>

	/* Internal */
#include "_internal_/_io.h"
#include "io.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

__attribute__((malloc))
char	*get_line(
	FILE *const _file
)
{
	char	*result;
	char	_buff[BUFFER_SIZE + 1];
	ssize_t	_s;

	while ((_s = fread(_buff, sizeof(char), BUFFER_SIZE, _file)) == BUFFER_SIZE)
	{
		char	*_pos = strchr(_buff, '\n');
		
	}


error:
	return (result);
}