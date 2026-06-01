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
	t_array *const	array,
	uint32_t		capacity
)
{
	array->data = mem_alloc(sizeof(void *) * capacity);
	if (unlikely(!array->data))
		return (error_alloc_fail);

	array->length = 0;
	array->capacity = capacity;

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
	void		**new_data;
	uint32_t	new_capacity;

	if (array->length >= array->capacity)
	{
		new_capacity = array->capacity + ALLOC_SIZE;

		new_data = mem_alloc(sizeof(void *) * new_capacity);
		if (unlikely(!new_data))
			return (error_alloc_fail);

		for (uint32_t i = 0; i < array->length; ++i)
			new_data[i] = array->data[i];

		mem_free(array->data);

		array->data = new_data;
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

	if (array_alloc(&result->dependencies, ALLOC_SIZE) || array_alloc(&result->defines, ALLOC_SIZE)
		|| array_alloc(&result->tags, ALLOC_SIZE) || array_alloc(&result->controls, ALLOC_SIZE))
	{
		array_free(&result->dependencies, false);
		array_free(&result->defines, false);
		array_free(&result->tags, false);
		array_free(&result->controls, false);

		mem_free(result);
		return (NULL);
	}

	return (result);
}
