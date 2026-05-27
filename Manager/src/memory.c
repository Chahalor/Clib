/**
 * 
*/
/* ----| Headers    |----- */
	/* Standard */
#include <errno.h>
#include <string.h>
#include <stdlib.h>

	/* Internal */
#include "types.h"
#include "manager.h"

	/* External */
#include "files/toml.h"
#include "interface/args.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

int	array_alloc(
	t_array *const	target,
	const uint32_t	capacity,
	const uint32_t	size_elts
)
{
	void	**r;

	r = mem_alloc(size_elts * capacity);
	if (unlikely(!r))
		return (error_alloc_fail);

	target->data = r;
	target->capacity = capacity;
	target->size = size_elts;
	target->length = 0;

	return (error_none);
}

void	array_free(
	t_array *const	array,
	const int		all
)
{
	if (all)
	{
		for (size_t i = 0; i < array->length; i++)
			mem_free(array->data[i]);
	}

	mem_free(array->data);
}

int	array_append(
	t_array *const	array,
	void *const		data
)
{
	if (unlikely(array->length + 1 >= array->size))
	{
		const uint32_t	new_capacity = array->capacity + ALLOC_SIZE;
		void			**new;

		new = mem_alloc(array->size * new_capacity);
		if (unlikely(!new))
			return (error_alloc_fail);

		for (size_t i = 0; i < array->length; i++)
			new[i] = array->data[i];

		mem_free(array->data);
		array->data = new;
		array->capacity = new_capacity;
	}

	array->data[array->length++] = data;
	return (error_none);
}

t_module	*module_new(void)
{
	t_module	*result;

	result = mem_alloc(sizeof(t_module));
	if (unlikely(!result))
		return (NULL);

	if (unlikely(array_alloc(&result->dependencies, ALLOC_SIZE, sizeof(char *))))
		goto cleannup;
	else if (unlikely(array_alloc(&result->defines, ALLOC_SIZE, sizeof(char *))))
		goto cleannup;
	else if (unlikely(array_alloc(&result->tags, ALLOC_SIZE, sizeof(char *))))
		goto cleannup;
	else if (unlikely(array_alloc(&result->controls, ALLOC_SIZE, sizeof(char *))))
		goto cleannup;

	return (result);

cleannup:
	mem_free(result->dependencies.data);
	mem_free(result->defines.data);
	mem_free(result->tags.data);
	mem_free(result->controls.data);
	mem_free(result);
	return (NULL);
}
