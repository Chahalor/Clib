#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../json.h"
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
		printf(" - [" RED "FAIL" RESET "] %s (rc=" YELLOW "%d" RESET ")\n", name, rc);
	}
}

static int	create_temp_file_with_content(
	const char *content,
	char *path_buf,
	size_t path_size
)
{
	const char	*template = "/tmp/clib_json_ctrl_XXXXXX";
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

static JSON	*fixture_json(void)
{
	return (json_load_str("{\"name\":\"bob\",\"age\":42,\"arr\":[1,2],\"obj\":{\"nested\":\"ok\"}}"));
}

static int	test_json_load_cases(void)
{
	char	path_valid[64];
	char	path_invalid[64];
	JSON	*json = NULL;

	EXPECT_EQ_INT(create_temp_file_with_content("{\"name\":\"alice\"}", path_valid, sizeof(path_valid)), 0);
	json = json_load(path_valid);
	EXPECT_NOT_NULL(json);
	EXPECT_STREQ((const char *)json_get(json, "name"), "alice");
	EXPECT_EQ_INT(json_unload(json), error_none);
	unlink(path_valid);

	EXPECT_EQ_INT(create_temp_file_with_content("{\"name\":", path_invalid, sizeof(path_invalid)), 0);
	EXPECT_NULL(json_load(path_invalid));
	unlink(path_invalid);

	EXPECT_NULL(json_load(NULL));
	return (0);
}

static int	test_json_load_str_cases(void)
{
	JSON	*json = NULL;

	json = json_load_str("{\"id\": %d, \"name\": \"%s\"}", 7, "alice");
	EXPECT_NOT_NULL(json);
	EXPECT_STREQ((const char *)json_get(json, "name"), "alice");
	EXPECT_EQ_INT(json_unload(json), error_none);

	EXPECT_NULL(json_load_str("{\"broken\": "));
	EXPECT_NULL(json_load_str(NULL));
	return (0);
}

static int	test_json_new_cases(void)
{
	JSON	*json = json_new();

	EXPECT_NOT_NULL(json);
	EXPECT_EQ_INT(json_get_type(json), json_tok_obj);
	EXPECT_NULL(json_get(json, "missing.field"));
	EXPECT_EQ_INT(json_unload(json), error_none);
	return (0);
}

static int	test_json_unload_cases(void)
{
	JSON	*json = fixture_json();
	JSON	*empty = json_new();

	EXPECT_NOT_NULL(json);
	EXPECT_EQ_INT(json_unload(json), error_none);

	EXPECT_NOT_NULL(empty);
	EXPECT_EQ_INT(json_unload(empty), error_none);

	EXPECT_EQ_INT(json_unload(NULL), -error_invalid_arg);
	return (0);
}

static int	test_json_get_cases(void)
{
	JSON	*json = fixture_json();

	EXPECT_NOT_NULL(json);
	EXPECT_STREQ((const char *)json_get(json, "name"), "bob");
	EXPECT_NULL(json_get(json, "missing.field"));
	EXPECT_NULL(json_get(NULL, "name"));
	EXPECT_NULL(json_get(json, NULL));

	EXPECT_EQ_INT(json_unload(json), error_none);
	return (0);
}

static int	test_json_sizeof_cases(void)
{
	JSON	*json = fixture_json();
	JSON	*empty = json_new();

	EXPECT_NOT_NULL(json);
	EXPECT_TRUE(json_sizeof(json) >= sizeof(JSON));

	EXPECT_NOT_NULL(empty);
	EXPECT_TRUE(json_sizeof(empty) >= sizeof(JSON));

	EXPECT_EQ_SIZE(json_sizeof(NULL), 0);

	EXPECT_EQ_INT(json_unload(json), error_none);
	EXPECT_EQ_INT(json_unload(empty), error_none);
	return (0);
}

static int	test_json_len_cases(void)
{
	JSON	*json = fixture_json();
	JSON	*arr = NULL;

	EXPECT_NOT_NULL(json);

	arr = (JSON *)json_get(json, "arr");
	EXPECT_NOT_NULL(arr);
	EXPECT_EQ_SIZE(json_len(arr), 0);

	EXPECT_EQ_SIZE(json_len((JSON *)json_get(json, "missing")), 0);
	EXPECT_EQ_SIZE(json_len(NULL), 0);

	EXPECT_EQ_INT(json_unload(json), error_none);
	return (0);
}

static int	test_json_set_cases(void)
{
	JSON	*json = json_new();
	int		age = 42;

	EXPECT_NOT_NULL(json);
	EXPECT_EQ_INT(json_set(json, "user.name", "bob", json_tok_str), error_none);
	EXPECT_STREQ((const char *)json_get(json, "user.name"), "bob");

	EXPECT_EQ_INT(json_set(json, "user.age", &age, json_tok_nbr), error_none);
	EXPECT_STREQ((const char *)json_get(json, "user.age"), "42");

	EXPECT_EQ_INT(json_set(json, NULL, "x", json_tok_str), error_invalid_arg);
	EXPECT_EQ_INT(json_set(NULL, "x", "y", json_tok_str), error_invalid_arg);
	EXPECT_EQ_INT(json_set_wild(json, "x", "y", 999), error_invalid_arg);

	EXPECT_EQ_INT(json_unload(json), error_none);
	return (0);
}

static int	test_json_set_typed_cases(void)
{
	JSON	*json = json_new();

	EXPECT_NOT_NULL(json);

	EXPECT_EQ_INT(json_set_int32(json, "users.%s.age", (int32_t)-12, "alice"), error_none);
	EXPECT_STREQ((const char *)json_get(json, "users.alice.age"), "-12");

	EXPECT_EQ_INT(json_set_uint32(json, "users.%s.id", (uint32_t)42, "alice"), error_none);
	EXPECT_STREQ((const char *)json_get(json, "users.alice.id"), "42");

	EXPECT_EQ_INT(json_set_int64(json, "metrics.%s.delta", (int64_t)-1234567890123LL, "cpu"), error_none);
	EXPECT_STREQ((const char *)json_get(json, "metrics.cpu.delta"), "-1234567890123");

	EXPECT_EQ_INT(json_set_uint64(json, "metrics.%s.total", (uint64_t)11325610020ULL, "cpu"), error_none);
	EXPECT_STREQ((const char *)json_get(json, "metrics.cpu.total"), "11325610020");

	EXPECT_EQ_INT(json_unload(json), error_none);
	return (0);
}

static int	test_json_set_string_format_cases(void)
{
	JSON	*json = json_new();

	EXPECT_NOT_NULL(json);

	EXPECT_EQ_INT(json_set_string(json, "profiles.%s.bio", "hello-%d", "bob", 7), error_none);
	EXPECT_STREQ((const char *)json_get(json, "profiles.bob.bio"), "hello-7");

	EXPECT_EQ_INT(json_set_string(json, "profiles.%s.tag", "%s:%d", "bob", "dev", 3), error_none);
	EXPECT_STREQ((const char *)json_get(json, "profiles.bob.tag"), "dev:3");

	EXPECT_EQ_INT(json_set_string(json, NULL, "x"), error_invalid_arg);
	EXPECT_EQ_INT(json_set_string(NULL, "x", "y"), error_invalid_arg);

	EXPECT_EQ_INT(json_unload(json), error_none);
	return (0);
}

static int	test_json_set_complex_format_cases(void)
{
	JSON	*json = json_new();
	JSON	*obj = json_load_str("{\"n\":1}");
	JSON	*arr = json_load_str("[1,2,3]");

	EXPECT_NOT_NULL(json);
	EXPECT_NOT_NULL(obj);
	EXPECT_NOT_NULL(arr);

	EXPECT_EQ_INT(json_set_obj(json, "slot.%s.obj", obj, "a"), error_none);
	EXPECT_STREQ((const char *)json_get(json, "slot.a.obj.n"), "1");

	EXPECT_EQ_INT(json_set_array(json, "slot.%s.arr", arr, "a"), error_none);
	EXPECT_STREQ((const char *)json_get(json, "slot.a.arr.2"), "3");

	EXPECT_EQ_INT(json_unload(obj), error_none);
	EXPECT_EQ_INT(json_unload(arr), error_none);
	EXPECT_EQ_INT(json_unload(json), error_none);
	return (0);
}

static int	test_json_set_from_array_cases(void)
{
	JSON	*json = json_new();
	int		nums[] = {1, 2, 3};

	EXPECT_NOT_NULL(json);
	EXPECT_EQ_INT(json_set_from_array(json, "numbers", nums, 3, json_tok_nbr), error_none);
	EXPECT_STREQ((const char *)json_get(json, "numbers.1"), "2");

	EXPECT_EQ_INT(json_set_from_array(json, "bad_numbers", NULL, 2, json_tok_nbr), error_invalid_arg);
	EXPECT_EQ_INT(json_set_from_array(json, NULL, nums, 3, json_tok_nbr), error_invalid_arg);

	EXPECT_EQ_INT(json_unload(json), error_none);
	return (0);
}

static int	test_json_array_append_cases(void)
{
	JSON	*json = json_load_str("{\"arr\":[\"a\"]}");
	JSON	*arr = NULL;

	EXPECT_NOT_NULL(json);
	arr = (JSON *)json_get(json, "arr");
	EXPECT_NOT_NULL(arr);

	EXPECT_EQ_INT(json_array_append(arr, "b", json_tok_str), error_none);
	EXPECT_STREQ((const char *)json_get(json, "arr.1"), "b");

	EXPECT_EQ_INT(json_array_append(arr, NULL, json_tok_null), error_none);
	EXPECT_NULL(json_get(json, "arr.2"));

	EXPECT_EQ_INT(json_array_append(arr, "c", 999), error_invalid_arg);

	EXPECT_EQ_INT(json_unload(json), error_none);
	return (0);
}

static int	test_json_array_pop_cases(void)
{
	JSON	*json = json_load_str("{\"arr\":[1,2,3]}");
	JSON	*arr = NULL;

	EXPECT_NOT_NULL(json);
	arr = (JSON *)json_get(json, "arr");
	EXPECT_NOT_NULL(arr);

	EXPECT_EQ_INT(json_array_pop(arr, 1), 0);
	EXPECT_STREQ((const char *)json_get(json, "arr.1"), "3");

	EXPECT_EQ_INT(json_array_pop(arr, 10), -1);
	EXPECT_EQ_INT(json_array_pop(arr, (size_t)-1), -1);

	EXPECT_EQ_INT(json_unload(json), error_none);
	return (0);
}

static int	test_json_unset_cases(void)
{
	JSON	*json = json_load_str("{\"user\":{\"name\":\"bob\"}}");

	EXPECT_NOT_NULL(json);

	EXPECT_EQ_INT(json_unset(json, "user.name"), error_none);
	EXPECT_NULL(json_get(json, "user.name"));

	EXPECT_EQ_INT(json_unset(json, "user.missing"), error_none);
	EXPECT_EQ_INT(json_unset(NULL, "user.name"), error_invalid_arg);
	EXPECT_EQ_INT(json_unset(json, NULL), error_invalid_arg);

	EXPECT_EQ_INT(json_unload(json), error_none);
	return (0);
}

static int	test_json_remove_cases(void)
{
	JSON	*json = json_load_str("{\"user\":{\"name\":\"bob\",\"age\":42}}");

	EXPECT_NOT_NULL(json);

	EXPECT_EQ_INT(json_remove(json, "user.age"), error_none);
	EXPECT_NULL(json_get(json, "user.age"));

	EXPECT_EQ_INT(json_remove(json, "user.missing"), error_none);
	EXPECT_EQ_INT(json_remove(NULL, "user.name"), error_invalid_arg);
	EXPECT_EQ_INT(json_remove(json, NULL), error_invalid_arg);

	EXPECT_EQ_INT(json_unload(json), error_none);
	return (0);
}

static int	test_json_dump_cases(void)
{
	JSON	*json = fixture_json();
	JSON	*empty = json_new();
	FILE	*file = NULL;
	char	buf[256];
	size_t	nread;

	EXPECT_NOT_NULL(json);
	file = tmpfile();
	EXPECT_NOT_NULL(file);
	EXPECT_EQ_INT(json_dump(json, file, 0), error_none);
	rewind(file);
	nread = fread(buf, 1, sizeof(buf) - 1, file);
	buf[nread] = '\0';
	EXPECT_CONTAINS(buf, "\"name\"");
	fclose(file);

	EXPECT_NOT_NULL(empty);
	file = tmpfile();
	EXPECT_NOT_NULL(file);
	EXPECT_EQ_INT(json_dump(empty, file, 0), error_none);
	rewind(file);
	nread = fread(buf, 1, sizeof(buf) - 1, file);
	buf[nread] = '\0';
	EXPECT_CONTAINS(buf, "{");
	fclose(file);

	EXPECT_EQ_INT(json_dump(NULL, stdout, 0), error_invalid_arg);
	EXPECT_EQ_INT(json_dump(json, NULL, 0), error_invalid_arg);

	EXPECT_EQ_INT(json_unload(json), error_none);
	EXPECT_EQ_INT(json_unload(empty), error_none);
	return (0);
}

static int	test_json_stringify_cases(void)
{
	JSON	*json = fixture_json();
	JSON	*empty = json_new();
	char	*str = NULL;

	EXPECT_NOT_NULL(json);
	str = json_stringify(json, 0);
	EXPECT_NOT_NULL(str);
	EXPECT_CONTAINS(str, "\"name\"");
	mem_free(str);

	EXPECT_NOT_NULL(empty);
	str = json_stringify(empty, 0);
	EXPECT_NOT_NULL(str);
	EXPECT_CONTAINS(str, "{");
	mem_free(str);

	EXPECT_NULL(json_stringify(NULL, 0));

	EXPECT_EQ_INT(json_unload(json), error_none);
	EXPECT_EQ_INT(json_unload(empty), error_none);
	return (0);
}

static int	test_json_assert_type_cases(void)
{
	JSON	*json = fixture_json();
	JSON	*obj = NULL;

	EXPECT_NOT_NULL(json);
	obj = (JSON *)json_get(json, "obj");
	EXPECT_NOT_NULL(obj);

	EXPECT_EQ_INT(json_assert_type(obj, json_tok_obj), 1);
	EXPECT_EQ_INT(json_assert_type(obj, json_tok_array), 0);
	EXPECT_EQ_INT(json_assert_type(NULL, json_tok_obj), 0);

	EXPECT_EQ_INT(json_unload(json), error_none);
	return (0);
}

static int	test_json_get_type_cases(void)
{
	JSON	*json = fixture_json();
	JSON	*missing = NULL;

	EXPECT_NOT_NULL(json);
	EXPECT_EQ_INT(json_get_type(json), json_tok_obj);

	missing = (JSON *)json_get(json, "obj.not_here");
	EXPECT_EQ_INT(json_get_type(missing), 0);
	EXPECT_EQ_INT(json_get_type(NULL), 0);

	EXPECT_EQ_INT(json_unload(json), error_none);
	return (0);
}

static int	test_json_array_append_obj(void)
{
	JSON	*json = json_new();
	JSON	*sub = json_load_str("{\"bob\": \"hey\", \"is\": 10, \"bob\": null}");

	EXPECT_NOT_NULL(json);
	EXPECT_NOT_NULL(sub);

	json_set(json, "tkt", "[]", json_tok_array);
	JSON	*array = json_get(json, "tkt");
	json_array_append(array, sub, json_tok_obj);
	json_array_append(array, sub, json_tok_obj);
	json_array_append(array, sub, json_tok_obj);
	json_array_append(array, sub, json_tok_obj);
	json_array_append(array, sub, json_tok_obj);

	json_foreach(j, json_get(json, "tkt"))
	{
		EXPECT_NOT_NULL(j);
	}

	return (0);
}

static int	json_set_0(void)
{
	JSON	*json = json_new();

	EXPECT_NOT_NULL(json);

	int	val = 0;
	json_set(json, "tkt", &val, json_tok_nbr);

	EXPECT_STREQ(json_get(json, "tkt"), "0");

	return (0);
}

static int	json_set_big_uint64_t(void)
{
	JSON		*json = json_new();
	uint64_t	val = 11325610020UL;

	EXPECT_NOT_NULL(json);

	json_set(json, "tkt", &val, json_tok_unbr);

	EXPECT_STREQ(json_get(json, "tkt"), "11325610020");

	return (0);
}

int	main(void)
{
	char	*color_total;

	run_test("json_load_cases", test_json_load_cases);
	run_test("json_load_str_cases", test_json_load_str_cases);
	run_test("json_new_cases", test_json_new_cases);
	run_test("json_unload_cases", test_json_unload_cases);
	run_test("json_get_cases", test_json_get_cases);
	run_test("json_sizeof_cases", test_json_sizeof_cases);
	run_test("json_len_cases", test_json_len_cases);
	run_test("json_set_cases", test_json_set_cases);
	run_test("json_set_typed_cases", test_json_set_typed_cases);
	run_test("json_set_string_format_cases", test_json_set_string_format_cases);
	run_test("json_set_complex_format_cases", test_json_set_complex_format_cases);
	run_test("json_set_from_array_cases", test_json_set_from_array_cases);
	run_test("json_array_append_cases", test_json_array_append_cases);
	run_test("json_array_pop_cases", test_json_array_pop_cases);
	run_test("json_unset_cases", test_json_unset_cases);
	run_test("json_remove_cases", test_json_remove_cases);
	run_test("json_dump_cases", test_json_dump_cases);
	run_test("json_stringify_cases", test_json_stringify_cases);
	run_test("json_assert_type_cases", test_json_assert_type_cases);
	run_test("json_get_type_cases", test_json_get_type_cases);
	// run_test("test_json_array_append_obj", test_json_array_append_obj);
	run_test("json_set_0", json_set_0);
	run_test("json_set_big_uint64_t", json_set_big_uint64_t);

	if (g_tests_failed == 0)
		color_total = GREEN;
	else if (g_tests_run - g_tests_failed != 0)
		color_total = YELLOW;
	else
		color_total = RED;

	printf("\n[SUMMARY] total=%s%d" RESET " passed=%s%d" RESET " failed=%d\n",
		color_total,
		g_tests_run,
		color_total,
		g_tests_run - g_tests_failed,
		g_tests_failed);

	return (g_tests_failed != 0);
}
