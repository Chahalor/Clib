/**
 * 
*/

#include <stdlib.h>

#include "../../math.h"
#include "../../random.h"
#include "../../types.h"
#include "../_internal_/_types.h"
#include "../_internal_/map/_map.h"
#include "../../standards/formating.h"

typedef struct s_heat_map
{
	int		*map;
	size_t	capacity;
	int		max;
	int		min;
}	HeatMap;

int	hash_map_add_random_entry(
	t_hash_map *const	map,
	void *const			value
)
{
	static const char	chars[] = \
		"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	char				key[33];

	if (unlikely(!map))
		return (error_invalid_arg);

	for (size_t i = 0; i < sizeof(key) - 1; i++)
		key[i] = chars[randint(0, (int)sizeof(chars) - 2)];
	key[sizeof(key) - 1] = '\0';

	return (hash_map_add(map, key, value));
}

HeatMap	*get_colition(
	t_hash_map *const map
)
{
	HeatMap	*result = NULL;

	result = calloc(1, sizeof(HeatMap) + sizeof(int) * map->capacity);
	if (unlikely(!result))
		return (NULL);

	result->map = (int *)(result + 1);
	result->capacity = map->capacity;

	for (size_t i = 0; i < map->capacity; i++)
	{
		t_hash_map_entry	*this = map->map[i];
		t_hash_map_entry	*next;

		if (!this)
			continue ;

		next = this;
		do
		{
			this = next;
			next = next->next;
			result->map[i]++;
			if (result->map[i] > result->max)
				result->max = result->map[i];

		} while (next);
	}

	result->min = MIN(result->map, int, result->capacity);
	return (result);
}

const char	*get_heat(
	const int	val,
	const int	min,
	const int	max
)
{
	static char	buff[32];
	float		t;
	int			r;
	int			g;
	int			b;

	if (max <= min)
		return ("\033[38;2;128;128;128m");

	t = (float)(val - min) / (float)(max - min);

	if (t < 0.0f)
		t = 0.0f;

	if (t > 1.0f)
		t = 1.0f;

	r = 128 + (int)(127.0f * t);
	g = 128 - (int)(128.0f * t);
	b = 128 - (int)(128.0f * t);

	snprintf(buff, sizeof(buff), "\033[38;2;%d;%d;%dm", r, g, b );
	return (buff);
}


void	display_HeatMap(
	const HeatMap *const map
)
{
	size_t	nb_rows;
	size_t	nb_cols;
	size_t	i;

	nb_rows = 1;
	while ((nb_rows + 1) * (nb_rows + 1) <= map->capacity)
		nb_rows++;

	nb_cols = (map->capacity + nb_rows - 1) / nb_rows;

	printf("{.capacity=%zu, .min=%d, .max=%d}\n", map->capacity, map->min, map->max);

	i = 0;
	for (size_t row = 0; row < nb_rows; ++row)
	{
		for (size_t col = 0; col < nb_cols; ++col)
		{
			if (i >= map->capacity)
				break;

			printf("%s██" RESET, get_heat(map->map[i], map->min, map->max));
			++i;
		}
		printf("\n");
	}
}

int	check_colitions(
	const int size_min,
	const int size_max
)
{
	for (size_t i = size_min; i < size_max; i++)
	{
		t_hash_map	map = {0};
		int			err = 0;
		HeatMap		*heat = NULL;

		if (i == 0)
			continue ;

		err = hash_map_setup(&map, i);
		if (unlikely(err))
		{
			printf("error %d occured for size %zu\n", err, i);
			continue ;
		}
		for (size_t entry = 0; entry < i; entry++)
		{
			err = hash_map_add_random_entry(&map, NULL);
			if (unlikely(err))
			{
				printf("error %d occured for entry %zu at size %zu\n",
					err, entry, i);
				break ;
			}
		}

		heat = get_colition(&map);
		display_HeatMap(heat);
		free(heat);
		hash_map_destroy(&map);
		setting->free(map.map);
	}

	return (error_none);
}

int main(int argc, char const *argv[])
{
	return check_colitions(4000, 4096);
}
