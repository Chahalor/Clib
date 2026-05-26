/**
 * @file _path.c
 * @brief TOML dotted path splitting helpers.
*/
/* ----| Headers    |----- */
	/* Standard */
#include <string.h>

	/* Internal */
#include "_toml.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static inline int	_toml_is_sep(
	const char c
)
{
	return (c == '.' || c == '/');
}

/* ----| Public     |----- */

int	_toml_count_field(
	const char *const field
)
{
	size_t	i = 0;
	int		count = 0;
	int		in_segment = 0;

	if (!field)
		return (0);

	while (field[i])
	{
		if (!_toml_is_sep(field[i]))
		{
			if (!in_segment)
				count++;
			in_segment = 1;
		}
		else
			in_segment = 0;
		i++;
	}

	return (count);
}

char	**_toml_split(
	const char *const field
)
{
	char	**result;
	char	*zone;
	size_t	len;
	size_t	i;
	size_t	part;
	size_t	zone_offset;
	int		count;

	if (unlikely(!field))
		return (NULL);

	len = strlen(field);
	count = _toml_count_field(field);
	result = mem_alloc(sizeof(char *) * (count + 1) + len + count + 1);
	if (unlikely(!result))
		return (NULL);

	zone = (char *)(result + count + 1);
	i = 0;
	part = 0;
	zone_offset = 0;
	while (field[i])
	{
		size_t	start;
		size_t	part_len;

		while (field[i] && _toml_is_sep(field[i]))
			i++;

		if (!field[i])
			break ;

		start = i;
		while (field[i] && !_toml_is_sep(field[i]))
			i++;

		part_len = i - start;
		result[part] = zone + zone_offset;
		memcpy(result[part], field + start, part_len);
		result[part][part_len] = '\0';
		zone_offset += part_len + 1;
		part++;
	}

	result[part] = NULL;
	return (result);
}

size_t	_toml_split_len(
	char **split
)
{
	size_t	len;

	len = 0;
	while (split && split[len])
		len++;

	return (len);
}
