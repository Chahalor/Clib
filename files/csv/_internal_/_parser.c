// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "_csv.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static int	_csv_parser_string(
	CSV **_csv,
	char *const restrict _buff,
	const size_t _length
)
{
	
}

/* ----| Public     |----- */

/**
 * TODO: do a gnl for evcery line of the file
 */
int	_csv_open(
	CSV **_csv,
	const char *const restrict _filename
)
{
	FILE	*_file;
	char	*_buff;
	size_t	_len_buff;
	int		result;

	_file = fopen(_filename, "r");
	if (unlikely(!_file))
		goto error;	// TODO: add error codes

	result = _csv_parser_string(_csv, _buff, _len_buff);

error:
	mem_free(_buff);
	return (result);
}