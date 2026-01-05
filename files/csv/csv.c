// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "csv.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

CSV	*csv_open(
	const char *const restrict _filename
)
{
	CSV	*result;

	if (unlikely(!_filename))
		return (NULL);

	result = _csv_open(_filename);

	return (result);
}

