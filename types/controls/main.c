/**
 * @file main.c
 * @brief Controls for the public API of the types module.
 */

#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../../types.h"
#include "../../standards/formating.h"

typedef int	(*t_test_fn)(void);

typedef struct s_control_test
{
	const char	*name;
	t_test_fn	fn;
}	t_control_test;

typedef struct s_type_control
{
	const char				*name;
	const t_control_test		*tests;
	size_t					count;
}	t_type_control;

static int	g_tests_run = 0;
static int	g_tests_failed = 0;

#define ARRAY_LEN(_array) (sizeof(_array) / sizeof((_array)[0]))

#define EXPECT_TRUE(_cond) \
	do { \
		if (!(_cond)) { \
			fprintf(stderr, " - assertion failed at " UNDERLINE "%s:%d" RESET ": %s\n", __FILE__, __LINE__, #_cond); \
			return (1); \
		} \
	} while (0)

#define EXPECT_FALSE(_cond) EXPECT_TRUE(!(_cond))
#define EXPECT_NULL(_ptr) EXPECT_TRUE((_ptr) == NULL)
#define EXPECT_NOT_NULL(_ptr) EXPECT_TRUE((_ptr) != NULL)

#define EXPECT_EQ_INT(_actual, _expected) \
	do { \
		long long _a = (long long)(_actual); \
		long long _e = (long long)(_expected); \
		if (_a != _e) { \
			fprintf(stderr, " - assertion failed at " UNDERLINE "%s:%d" RESET ": got=%lld expected=%lld\n", __FILE__, __LINE__, _a, _e); \
			return (1); \
		} \
	} while (0)

#define EXPECT_EQ_SIZE(_actual, _expected) \
	do { \
		size_t _a = (size_t)(_actual); \
		size_t _e = (size_t)(_expected); \
		if (_a != _e) { \
			fprintf(stderr, " - assertion failed at " UNDERLINE "%s:%d" RESET ": got=%zu expected=%zu\n", __FILE__, __LINE__, _a, _e); \
			return (1); \
		} \
	} while (0)

static void	*_control_alloc(size_t size)
{
	return (calloc(1, size));
}

static void	*_control_realloc(void *ptr, size_t size)
{
	return (realloc(ptr, size));
}

static void	_control_free(void *ptr)
{
	free(ptr);
}

static void	*_control_dup(const void *ptr, ssize_t size)
{
	void	*dup;

	if (!ptr || size < 0)
		return (NULL);
	dup = malloc((size_t)size);
	if (!dup)
		return (NULL);
	return (memcpy(dup, ptr, (size_t)size));
}

static size_t	_control_hash(const char *const str)
{
	const char	*cursor;
	size_t		hash;

	hash = 1469598103934665603ULL;
	if (!str)
		return (0);
	cursor = str;
	while (*cursor)
	{
		hash ^= (unsigned char)*cursor;
		hash *= 1099511628211ULL;
		cursor++;
	}
	return (hash);
}

static size_t	_control_collision_hash(const char *const str)
{
	if (str && (!strcmp(str, "first") || !strcmp(str, "second")
			|| !strcmp(str, "third")))
		return (42);
	return (_control_hash(str));
}

static void	_install_type_controls(size_t (*hash)(const char *const))
{
	g_types_settings.error.errnum = error_none;
	g_types_settings.error.key = NULL;
	g_types_settings.alloc = _control_alloc;
	g_types_settings.realloc = _control_realloc;
	g_types_settings.free = _control_free;
	g_types_settings.dup = _control_dup;
	g_types_settings.hash_string = hash;
}

static int	_map_setup(t_hash_map *map, size_t capacity)
{
	if (!map || capacity == 0)
		return (1);
	memset(map, 0, sizeof(*map));
	map->map = calloc(capacity, sizeof(*map->map));
	if (!map->map)
		return (1);
	map->capacity = capacity;
	return (0);
}

static void	_map_destroy(t_hash_map *map, bool free_values)
{
	t_hash_map_entry	*entry;
	t_hash_map_entry	*next;

	if (!map || !map->map)
		return ;
	for (size_t i = 0; i < map->capacity; i++)
	{
		entry = map->map[i];
		while (entry)
		{
			next = entry->next;
			if (free_values)
				free(entry->value);
			free(entry);
			entry = next;
		}
	}
	free(map->map);
	memset(map, 0, sizeof(*map));
}

static int	test_hash_map_invalid_args(void)
{
	t_hash_map	map;
	int			value;

	_install_type_controls(_control_hash);
	EXPECT_EQ_INT(_map_setup(&map, 8), 0);
	value = 42;
	EXPECT_EQ_INT(hash_map_add(NULL, "key", &value), error_invalid_arg);
	EXPECT_EQ_INT(hash_map_add(&map, NULL, &value), error_invalid_arg);
	EXPECT_NULL(hash_map_find(NULL, "key"));
	EXPECT_NULL(hash_map_find(&map, NULL));
	EXPECT_NULL(hash_map_find_ptr(NULL, "key"));
	EXPECT_NULL(hash_map_find_ptr(&map, NULL));
	EXPECT_FALSE(hash_map_contains(NULL, "key"));
	EXPECT_FALSE(hash_map_contains(&map, NULL));
	EXPECT_EQ_INT(hash_map_replace(NULL, "key", &value), error_invalid_arg);
	EXPECT_EQ_INT(hash_map_replace(&map, NULL, &value), error_invalid_arg);
	hash_map_remove(NULL, "key");
	EXPECT_EQ_INT(g_types_settings.error.errnum, error_invalid_arg);
	hash_map_remove(&map, NULL);
	EXPECT_EQ_INT(g_types_settings.error.errnum, error_invalid_arg);
	EXPECT_EQ_INT(hash_map_resize(NULL, 8), error_invalid_arg);
	EXPECT_EQ_INT(hash_map_resize(&map, 0), error_invalid_arg);
	_map_destroy(&map, false);
	return (0);
}

static int	test_hash_map_add_find_contains(void)
{
	t_hash_map	map;
	int			alpha;
	int			beta;

	_install_type_controls(_control_hash);
	EXPECT_EQ_INT(_map_setup(&map, 17), 0);
	alpha = 10;
	beta = 20;
	EXPECT_EQ_INT(hash_map_add(&map, "alpha", &alpha), error_none);
	EXPECT_EQ_INT(hash_map_add(&map, "beta", &beta), error_none);
	EXPECT_EQ_SIZE(map.length, 2);
	EXPECT_TRUE(hash_map_contains(&map, "alpha"));
	EXPECT_TRUE(hash_map_contains(&map, "beta"));
	EXPECT_FALSE(hash_map_contains(&map, "missing"));
	EXPECT_TRUE(hash_map_find(&map, "alpha") == &alpha);
	EXPECT_TRUE(hash_map_find(&map, "beta") == &beta);
	EXPECT_NULL(hash_map_find(&map, "missing"));
	_map_destroy(&map, false);
	return (0);
}

static int	test_hash_map_find_ptr_updates_value(void)
{
	t_hash_map	map;
	int			before;
	int			after;
	void		**slot;

	_install_type_controls(_control_hash);
	EXPECT_EQ_INT(_map_setup(&map, 17), 0);
	before = 11;
	after = 22;
	EXPECT_EQ_INT(hash_map_add(&map, "item", &before), error_none);
	slot = hash_map_find_ptr(&map, "item");
	EXPECT_NOT_NULL(slot);
	EXPECT_TRUE(*slot == &before);
	*slot = &after;
	EXPECT_TRUE(hash_map_find(&map, "item") == &after);
	EXPECT_NULL(hash_map_find_ptr(&map, "missing"));
	_map_destroy(&map, false);
	return (0);
}

static int	test_hash_map_replace(void)
{
	t_hash_map	map;
	int			old_value;
	int			new_value;
	int			other_value;

	_install_type_controls(_control_hash);
	EXPECT_EQ_INT(_map_setup(&map, 17), 0);
	old_value = 1;
	new_value = 2;
	other_value = 3;
	EXPECT_EQ_INT(hash_map_add(&map, "replace-me", &old_value), error_none);
	EXPECT_EQ_INT(hash_map_replace(&map, "replace-me", &new_value), error_none);
	EXPECT_TRUE(hash_map_find(&map, "replace-me") == &new_value);
	EXPECT_EQ_INT(hash_map_replace(&map, "missing", &other_value), -1);
	EXPECT_EQ_SIZE(map.length, 1);
	_map_destroy(&map, false);
	return (0);
}

static int	test_hash_map_remove(void)
{
	t_hash_map	map;
	int			first;
	int			second;
	int			third;

	_install_type_controls(_control_hash);
	EXPECT_EQ_INT(_map_setup(&map, 17), 0);
	first = 1;
	second = 2;
	third = 3;
	EXPECT_EQ_INT(hash_map_add(&map, "first", &first), error_none);
	EXPECT_EQ_INT(hash_map_add(&map, "second", &second), error_none);
	EXPECT_EQ_INT(hash_map_add(&map, "third", &third), error_none);
	hash_map_remove(&map, "second");
	EXPECT_EQ_SIZE(map.length, 2);
	EXPECT_FALSE(hash_map_contains(&map, "second"));
	EXPECT_TRUE(hash_map_find(&map, "first") == &first);
	EXPECT_TRUE(hash_map_find(&map, "third") == &third);
	hash_map_remove(&map, "missing");
	EXPECT_EQ_SIZE(map.length, 2);
	_map_destroy(&map, false);
	return (0);
}

static int	test_hash_map_collisions(void)
{
	t_hash_map	map;
	int			first;
	int			second;
	int			third;

	_install_type_controls(_control_collision_hash);
	EXPECT_EQ_INT(_map_setup(&map, 5), 0);
	first = 10;
	second = 20;
	third = 30;
	EXPECT_EQ_INT(hash_map_add(&map, "first", &first), error_none);
	EXPECT_EQ_INT(hash_map_add(&map, "second", &second), error_none);
	EXPECT_EQ_INT(hash_map_add(&map, "third", &third), error_none);
	EXPECT_EQ_SIZE(map.length, 3);
	EXPECT_TRUE(hash_map_find(&map, "first") == &first);
	EXPECT_TRUE(hash_map_find(&map, "second") == &second);
	EXPECT_TRUE(hash_map_find(&map, "third") == &third);
	EXPECT_FALSE(hash_map_contains(&map, "fourth"));
	_map_destroy(&map, false);
	return (0);
}

static int	test_hash_map_resize_keeps_entries(void)
{
	t_hash_map	map;
	int			alpha;
	int			beta;
	int			gamma;

	_install_type_controls(_control_hash);
	EXPECT_EQ_INT(_map_setup(&map, 3), 0);
	alpha = 100;
	beta = 200;
	gamma = 300;
	EXPECT_EQ_INT(hash_map_add(&map, "alpha", &alpha), error_none);
	EXPECT_EQ_INT(hash_map_add(&map, "beta", &beta), error_none);
	EXPECT_EQ_INT(hash_map_add(&map, "gamma", &gamma), error_none);
	EXPECT_EQ_INT(hash_map_resize(&map, 31), error_none);
	EXPECT_EQ_SIZE(map.capacity, 31);
	EXPECT_EQ_SIZE(map.length, 3);
	EXPECT_TRUE(hash_map_find(&map, "alpha") == &alpha);
	EXPECT_TRUE(hash_map_find(&map, "beta") == &beta);
	EXPECT_TRUE(hash_map_find(&map, "gamma") == &gamma);
	_map_destroy(&map, false);
	return (0);
}

static void	print_type_header(const char *name)
{
	printf("\n" BOLD CYAN "============================================================" RESET "\n");
	printf(BOLD CYAN "  %s" RESET "\n", name);
	printf(BOLD CYAN "============================================================" RESET "\n");
}

static int	run_test_in_child(t_test_fn test)
{
	pid_t	pid;
	int		status;

	fflush(stdout);
	fflush(stderr);
	pid = fork();
	if (pid < 0)
		return (test());
	if (pid == 0)
		exit(test() == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
	if (waitpid(pid, &status, 0) < 0)
		return (1);
	if (WIFSIGNALED(status))
	{
		fprintf(stderr, " - child stopped by signal %d\n", WTERMSIG(status));
		return (1);
	}
	if (!WIFEXITED(status))
		return (1);
	return (WEXITSTATUS(status));
}

static void	run_test(const char *name, t_test_fn test)
{
	int	rc;

	g_tests_run++;
	printf("[" BLUE "TEST" RESET "] %s\n", name);
	rc = run_test_in_child(test);
	if (rc == 0)
		printf(" - [" GREEN "PASS" RESET "] %s\n", name);
	else
	{
		g_tests_failed++;
		printf(" - [" RED "FAIL" RESET "] %s (rc=" YELLOW "%d" RESET ")\n", name, rc);
	}
}

static void	run_type_controls(const t_type_control *control)
{
	print_type_header(control->name);
	for (size_t i = 0; i < control->count; i++)
		run_test(control->tests[i].name, control->tests[i].fn);
}

static const t_control_test	g_hash_map_tests[] = {
	{"invalid argument guards", test_hash_map_invalid_args},
	{"add, find and contains", test_hash_map_add_find_contains},
	{"find pointer updates value", test_hash_map_find_ptr_updates_value},
	{"replace existing value", test_hash_map_replace},
	{"remove entries", test_hash_map_remove},
	{"collision chains", test_hash_map_collisions},
	{"resize keeps entries", test_hash_map_resize_keeps_entries},
};

static const t_type_control	g_type_controls[] = {
	{"Hash map", g_hash_map_tests, ARRAY_LEN(g_hash_map_tests)},
};

int	main(void)
{
	for (size_t i = 0; i < ARRAY_LEN(g_type_controls); i++)
		run_type_controls(&g_type_controls[i]);
	printf("\n" BOLD "Summary:" RESET " %d run, %d passed, %d failed\n",
		g_tests_run, g_tests_run - g_tests_failed, g_tests_failed);
	if (g_tests_failed)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}
