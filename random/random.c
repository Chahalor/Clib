// Header

/* ----| Headers    |----- */
	/* Standard */
#include <stdlib.h>
#include <time.h>

	/* Internal */
#include "random.h"

	/* External */
#include "../math/math.h"
#include "../memory/memory.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

/**
 * @brief	init the srand() has a constructor
 * 
 * @version	1.0
 */
__attribute__((constructor))
static void	random_init_seed(void)
{
	srand(time(NULL));
}

/* ----| Public     |----- */

// __attribute__(())
int	randint(
	const int _val1,
	const int _val2
)
{
	const int	_val = rand();
	const int	_min = min(_val1, _val2);
	const int	_max = max(_val1, _val2);

	return (_min + (_val % ((_max - _min) + 1)));
}

__attribute__((malloc))
int	*rand_range(
	const unsigned int _size,
	const int _val1,
	const int _val2
)
{
	int				*result = NULL;
	unsigned int	_i = 0;

	result = mem_alloc((_size + 1) * sizeof(int));
	if (unlikely(!result))
		return (NULL);
	while (_i < _size)
		result[_i++] = randint(_val1, _val2);
	result[_size] = 0;
	return (result);
}
