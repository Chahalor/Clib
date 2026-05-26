#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../toml.h"
#include "../../../memory.h"
#include "../../../standards/formating.h"

typedef int	(*t_test_fn)(void);

static int	g_tests_run = 0;
static int	g_tests_failed = 0;

#define EXPECT_TRUE(cond) \
	do { \
		if (!(cond)) { \
			fprintf(stderr, " - assertion failed at " UNDERLINE "%s:%d" RESET ": %s\n", __FILE__, __LINE__, #cond); \
			return (1); \
		} \
	} while (0)

#define EXPECT_NOT_NULL(ptr) EXPECT_TRUE((ptr) != NULL)
#define EXPECT_NULL(ptr) EXPECT_TRUE((ptr) == NULL)

#define EXPECT_EQ_INT(actual, expected) \
	do { \
		long long _a = (long long)(actual); \
		long long _e = (long long)(expected); \
		if (_a != _e) { \
			fprintf(stderr, " - assertion failed at " UNDERLINE "%s:%d" RESET ": got=%lld expected=%lld\n", __FILE__, __LINE__, _a, _e); \
			return (1); \
		} \
	} while (0)

#define EXPECT_EQ_SIZE(actual, expected) \
	do { \
		size_t _a = (size_t)(actual); \
		size_t _e = (size_t)(expected); \
		if (_a != _e) { \
			fprintf(stderr, " - assertion failed at " UNDERLINE "%s:%d" RESET ": got=%zu expected=%zu\n", __FILE__, __LINE__, _a, _e); \
			return (1); \
		} \
	} while (0)

#define EXPECT_STREQ(actual, expected) \
	do { \
		const char *_a = (actual); \
		const char *_e = (expected); \
		if (!_a || !_e || strcmp(_a, _e)) { \
			fprintf(stderr, " - assertion failed at " UNDERLINE "%s:%d" RESET ": got=\"%s\" expected=\"%s\"\n", __FILE__, __LINE__, _a ? _a : "(null)", _e ? _e : "(null)"); \
			return (1); \
		} \
	} while (0)

#define EXPECT_CONTAINS(haystack, needle) \
	do { \
		const char *_h = (haystack); \
		const char *_n = (needle); \
		if (!_h || !_n || strstr(_h, _n) == NULL) { \
			fprintf(stderr, " - assertion failed at " UNDERLINE "%s:%d" RESET ": \"%s\" does not contain \"%s\"\n", __FILE__, __LINE__, _h ? _h : "(null)", _n ? _n : "(null)"); \
			return (1); \
		} \
	} while (0)

static void	run_test(
	const char *name,
	t_test_fn test
)
{
	int	rc;

	g_tests_run++;
	printf("[" BLUE "TEST" RESET "] %s\n", name);
	rc = test();
	if (rc == 0)
		printf(" - [" GREEN "PASS" RESET "] %s\n", name);
	else
	{
		g_tests_failed++;
		printf(" - [" RED "FAIL" RESET "] %s (rc=" YELLOW "%d" RESET ")\n",
			name, rc);
	}
}

static int	create_temp_file_with_content(
	const char *content,
	char *path_buf,
	size_t path_size
)
{
	const char	*template = "/tmp/clib_toml_ctrl_XXXXXX";
	size_t		template_len = strlen(template);
	int			fd;
	FILE		*file;
	size_t		written;

	if (!content || !path_buf || path_size <= template_len)
		return (1);
	memcpy(path_buf, template, template_len + 1);
	fd = mkstemp(path_buf);
	if (fd < 0)
		return (1);
	file = fdopen(fd, "w");
	if (!file)
		return (close(fd), unlink(path_buf), 1);
	written = fwrite(content, 1, strlen(content), file);
	if (written != strlen(content))
		return (fclose(file), unlink(path_buf), 1);
	if (fclose(file) != 0)
		return (unlink(path_buf), 1);
	return (0);
}

static TOML	*fixture_toml(void)
{
	return (toml_load_str(
			"name = \"bob\"\n"
			"age = 42\n"
			"active = true\n"
			"arr = [1, 2]\n"
			"[obj]\n"
			"nested = \"ok\"\n"));
}

static int	test_toml_load_cases(void)
{
	char	path_valid[64];
	TOML	*toml;

	EXPECT_EQ_INT(create_temp_file_with_content(
			"name = \"alice\"\n", path_valid, sizeof(path_valid)), 0);
	toml = toml_load(path_valid);
	EXPECT_NOT_NULL(toml);
	EXPECT_STREQ((const char *)toml_get(toml, "name"), "alice");
	EXPECT_EQ_INT(toml_unload(toml), error_none);
	unlink(path_valid);

	EXPECT_NULL(toml_load(NULL));
	return (0);
}

static int	test_toml_load_str_cases(void)
{
	TOML	*toml;

	toml = toml_load_str("id = %d\nname = \"%s\"\n", 7, "alice");
	EXPECT_NOT_NULL(toml);
	EXPECT_STREQ((const char *)toml_get(toml, "id"), "7");
	EXPECT_STREQ((const char *)toml_get(toml, "name"), "alice");
	EXPECT_EQ_INT(toml_unload(toml), error_none);

	EXPECT_NULL(toml_load_str(NULL));
	return (0);
}

static int	test_toml_new_unload_cases(void)
{
	TOML	*toml;

	toml = toml_new();
	EXPECT_NOT_NULL(toml);
	EXPECT_EQ_INT(toml_get_type(toml), toml_tok_table);
	EXPECT_NULL(toml_get(toml, "missing"));
	EXPECT_EQ_INT(toml_unload(toml), error_none);
	EXPECT_EQ_INT(toml_unload(NULL), -error_invalid_arg);
	return (0);
}

static int	test_toml_get_cases(void)
{
	TOML	*toml;

	toml = fixture_toml();
	EXPECT_NOT_NULL(toml);
	EXPECT_STREQ((const char *)toml_get(toml, "name"), "bob");
	EXPECT_STREQ((const char *)toml_get(toml, "%s.nested", "obj"), "ok");
	EXPECT_NULL(toml_get(toml, "missing.field"));
	EXPECT_NULL(toml_get(NULL, "name"));
	EXPECT_NULL(toml_get(toml, NULL));
	EXPECT_EQ_INT(toml_unload(toml), error_none);
	return (0);
}

static int	test_toml_sizeof_len_cases(void)
{
	TOML	*toml;
	TOML	*arr;

	toml = fixture_toml();
	EXPECT_NOT_NULL(toml);
	EXPECT_TRUE(toml_sizeof(toml) >= sizeof(TOML));
	EXPECT_TRUE(toml_len(toml) >= 4);

	arr = (TOML *)toml_get(toml, "arr");
	EXPECT_NOT_NULL(arr);
	EXPECT_EQ_SIZE(toml_len(arr), 2);
	EXPECT_EQ_SIZE(toml_sizeof(NULL), 0);
	EXPECT_EQ_SIZE(toml_len(NULL), 0);
	EXPECT_EQ_INT(toml_unload(toml), error_none);
	return (0);
}

static int	test_toml_set_cases(void)
{
	TOML	*toml;
	int		age;
	int		active;

	toml = toml_new();
	age = 42;
	active = 1;
	EXPECT_NOT_NULL(toml);
	EXPECT_EQ_INT(toml_set(toml, "user.name", "bob", toml_str), error_none);
	EXPECT_STREQ((const char *)toml_get(toml, "user.name"), "bob");
	EXPECT_EQ_INT(toml_set(toml, "user.age", age), error_none);
	EXPECT_STREQ((const char *)toml_get(toml, "user.age"), "42");
	EXPECT_EQ_INT(toml_set_wild(toml, "user.active", &active, toml_bool),
		error_none);
	EXPECT_STREQ((const char *)toml_get(toml, "user.active"), "true");
	EXPECT_EQ_INT(toml_set(toml, NULL, "x", toml_str), error_invalid_arg);
	EXPECT_EQ_INT(toml_set(NULL, "x", "y", toml_str), error_invalid_arg);
	EXPECT_EQ_INT(toml_set_wild(toml, "x", "y", 999), error_invalid_arg);
	EXPECT_EQ_INT(toml_unload(toml), error_none);
	return (0);
}

static int	test_toml_set_format_cases(void)
{
	TOML	*toml;

	toml = toml_new();
	EXPECT_NOT_NULL(toml);
	EXPECT_EQ_INT(toml_set(toml, "users.%s.age", (int32_t)-12, "alice"),
		error_none);
	EXPECT_STREQ((const char *)toml_get(toml, "users.alice.age"), "-12");
	EXPECT_EQ_INT(toml_set(toml, "users.%s.id", (uint32_t)42, "alice"),
		error_none);
	EXPECT_STREQ((const char *)toml_get(toml, "users.alice.id"), "42");
	EXPECT_EQ_INT(toml_set_string(toml, "profiles.%s.bio", "hello-%d",
			"bob", 7), error_none);
	EXPECT_STREQ((const char *)toml_get(toml, "profiles.bob.bio"), "hello-7");
	EXPECT_EQ_INT(toml_unload(toml), error_none);
	return (0);
}

static int	test_toml_array_cases(void)
{
	TOML	*toml;
	TOML	*arr;
	int		nums[3] = {1, 2, 3};
	int		value;

	toml = toml_new();
	value = 4;
	EXPECT_NOT_NULL(toml);
	EXPECT_EQ_INT(toml_set_from_array(toml, "numbers", nums, 3, toml_int),
		error_none);
	EXPECT_STREQ((const char *)toml_get(toml, "numbers.1"), "2");
	arr = (TOML *)toml_get(toml, "numbers");
	EXPECT_NOT_NULL(arr);
	EXPECT_EQ_SIZE(toml_len(arr), 3);
	EXPECT_EQ_INT(toml_array_append(arr, &value, toml_int), error_none);
	EXPECT_STREQ((const char *)toml_get(toml, "numbers.3"), "4");
	EXPECT_EQ_INT(toml_array_pop(arr, 1), error_none);
	EXPECT_STREQ((const char *)toml_get(toml, "numbers.1"), "3");
	EXPECT_EQ_INT(toml_array_append(arr, "bad", 999), error_invalid_arg);
	EXPECT_EQ_INT(toml_set_from_array(toml, "bad", NULL, 2, toml_int),
		error_invalid_arg);
	EXPECT_EQ_INT(toml_unload(toml), error_none);
	return (0);
}

static int	test_toml_unset_remove_cases(void)
{
	TOML	*toml;

	toml = toml_load_str("[user]\nname = \"bob\"\nage = 42\n");
	EXPECT_NOT_NULL(toml);
	EXPECT_EQ_INT(toml_unset(toml, "user.name"), error_none);
	EXPECT_NULL(toml_get(toml, "user.name"));
	EXPECT_EQ_INT(toml_remove(toml, "user.age"), error_none);
	EXPECT_NULL(toml_get(toml, "user.age"));
	EXPECT_EQ_INT(toml_unset(toml, "user.missing"), error_none);
	EXPECT_EQ_INT(toml_unset(NULL, "user.name"), error_invalid_arg);
	EXPECT_EQ_INT(toml_remove(toml, NULL), error_invalid_arg);
	EXPECT_EQ_INT(toml_unload(toml), error_none);
	return (0);
}

static int	test_toml_stringify_dump_cases(void)
{
	TOML	*toml;
	FILE	*file;
	char	*str;
	char	buf[256];
	size_t	nread;

	toml = fixture_toml();
	EXPECT_NOT_NULL(toml);
	str = toml_stringify(toml, 2);
	EXPECT_NOT_NULL(str);
	EXPECT_CONTAINS(str, "name = \"bob\"");
	EXPECT_CONTAINS(str, "[obj]");
	mem_free(str);

	file = tmpfile();
	EXPECT_NOT_NULL(file);
	EXPECT_EQ_INT(toml_dump(toml, file, 2), error_none);
	rewind(file);
	nread = fread(buf, 1, sizeof(buf) - 1, file);
	buf[nread] = '\0';
	EXPECT_CONTAINS(buf, "active = true");
	fclose(file);
	EXPECT_EQ_INT(toml_dump(NULL, stdout, 0), error_invalid_arg);
	EXPECT_EQ_INT(toml_dump(toml, NULL, 0), error_invalid_arg);
	EXPECT_EQ_INT(toml_unload(toml), error_none);
	return (0);
}

static int	test_toml_type_foreach_cases(void)
{
	TOML	*toml;
	TOML	*arr;
	size_t	count;

	toml = fixture_toml();
	EXPECT_NOT_NULL(toml);
	EXPECT_EQ_INT(toml_get_type(toml), toml_tok_table);
	arr = (TOML *)toml_get(toml, "arr");
	EXPECT_NOT_NULL(arr);
	EXPECT_EQ_INT(toml_assert_type(arr, toml_tok_array), 1);
	EXPECT_EQ_INT(toml_assert_type(arr, toml_tok_table), 0);
	count = 0;
	toml_foreach(node, arr)
	{
		EXPECT_NOT_NULL(node);
		count++;
	}
	EXPECT_EQ_SIZE(count, 2);
	EXPECT_EQ_INT(toml_get_type(NULL), 0);
	EXPECT_EQ_INT(toml_assert_type(NULL, toml_tok_array), 0);
	EXPECT_EQ_INT(toml_unload(toml), error_none);
	return (0);
}

int	main(void)
{
	char	*color_total;

	run_test("toml_load_cases", test_toml_load_cases);
	run_test("toml_load_str_cases", test_toml_load_str_cases);
	run_test("toml_new_unload_cases", test_toml_new_unload_cases);
	run_test("toml_get_cases", test_toml_get_cases);
	run_test("toml_sizeof_len_cases", test_toml_sizeof_len_cases);
	run_test("toml_set_cases", test_toml_set_cases);
	run_test("toml_set_format_cases", test_toml_set_format_cases);
	run_test("toml_array_cases", test_toml_array_cases);
	run_test("toml_unset_remove_cases", test_toml_unset_remove_cases);
	run_test("toml_stringify_dump_cases", test_toml_stringify_dump_cases);
	run_test("toml_type_foreach_cases", test_toml_type_foreach_cases);

	if (g_tests_failed == 0)
		color_total = GREEN;
	else if (g_tests_run - g_tests_failed != 0)
		color_total = YELLOW;
	else
		color_total = RED;

	printf("\n[SUMMARY] total=%s%d" RESET " passed=%s%d" RESET
		" failed=%d\n",
		color_total,
		g_tests_run,
		color_total,
		g_tests_run - g_tests_failed,
		g_tests_failed);
	return (g_tests_failed != 0);
}
