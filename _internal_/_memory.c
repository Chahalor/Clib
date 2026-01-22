// Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdlib.h>

	/* Internal */
#include "_config_.h"
#include "_internal_.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

__attribute__((visibility("hidden")))
void	*_mem_alloc_(
	const size_t size
)
{
	void	*result;

	result = malloc(size);
	if (unlikely(!result))
		_internal_get_config_()->errnum = error_alloc_fail;
	return (result);
}

__attribute__((visibility("hidden")))
void	_mem_free_(
	void *const restrict ptr
)
{
	free(ptr);
}
