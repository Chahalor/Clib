#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../../HTTP.h"
#include "../../../../memory.h"
#include "../_internal_/_HTTP.h"
#include "../../../../standards/formating.h"

typedef int	(*t_test_fn)(void);

static int	g_tests_run = 0;
static int	g_tests_failed = 0;

#define EXPECT_TRUE(cond) \
	do { \
		if (!(cond)) { \
			fprintf(stderr, "  assertion failed at %s:%d: %s\n", __FILE__, __LINE__, #cond); \
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
			fprintf(stderr, "  assertion failed at %s:%d: got=%lld expected=%lld\n", __FILE__, __LINE__, _a, _e); \
			return (1); \
		} \
	} while (0)

#define EXPECT_STREQ(actual, expected) \
	do { \
		const char *_a = (actual); \
		const char *_e = (expected); \
		if (!_a || !_e || strcmp(_a, _e)) { \
			fprintf(stderr, "  assertion failed at %s:%d: got=\"%s\" expected=\"%s\"\n", __FILE__, __LINE__, _a ? _a : "(null)", _e ? _e : "(null)"); \
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
		printf(" - [" GREEN "PASS" RESET"] %s\n", name);
	else
	{
		g_tests_failed++;
		printf(" - [" RED "FAIL" RESET "] %s (rc=" YELLOW "%d" RESET ")\n", name, rc);
	}
}

static int	test_parse_version_valid(void)
{
	EXPECT_EQ_INT(_http_parse_version("HTTP/1.0"), HTTP_1_0);
	EXPECT_EQ_INT(_http_parse_version("HTTP/1.1"), HTTP_1_1);
	EXPECT_EQ_INT(_http_parse_version("HTTP/1.0\r\n"), HTTP_1_0);
	return (0);
}

static int	test_parse_version_invalid(void)
{
	EXPECT_EQ_INT(_http_parse_version(NULL), 0);
	EXPECT_EQ_INT(_http_parse_version(""), 0);
	EXPECT_EQ_INT(_http_parse_version("HTP/1.0"), 0);
	EXPECT_EQ_INT(_http_parse_version("HTTP/1"), 0);
	EXPECT_EQ_INT(_http_parse_version("HTTP/A.0"), 0);
	EXPECT_EQ_INT(_http_parse_version("HTTP/1.0 extra"), 0);
	return (0);
}

static int	test_version_to_str(void)
{
	char	buf[16];

	EXPECT_NOT_NULL(_http_version_to_str(HTTP_1_0, buf));
	EXPECT_STREQ(buf, "HTTP/1.0");
	EXPECT_NULL(_http_version_to_str(HTTP_1_0, NULL));
	return (0);
}

static int	test_str_to_nb(void)
{
	char	*end;

	EXPECT_EQ_INT(_str_to_nb("42abc", &end), 42);
	EXPECT_STREQ(end, "abc");
	EXPECT_EQ_INT(_str_to_nb("-7", &end), -7);
	EXPECT_STREQ(end, "");
	EXPECT_EQ_INT(_str_to_nb("+19 ", &end), 19);
	EXPECT_TRUE(*end == ' ');
	return (0);
}

static int	test_next_line(void)
{
	const char	*raw = "GET / HTTP/1.0\r\nHost: example.com\r\n\r\n";
	const char	*line = NULL;
	size_t		offset = 0;
	size_t		line_len = 0;

	EXPECT_EQ_INT(_next_line(raw, strlen(raw), &offset, &line, &line_len), 1);
	EXPECT_EQ_INT((int)line_len, 14);
	EXPECT_TRUE(!strncmp(line, "GET / HTTP/1.0", line_len));

	EXPECT_EQ_INT(_next_line(raw, strlen(raw), &offset, &line, &line_len), 1);
	EXPECT_EQ_INT((int)line_len, 17);
	EXPECT_TRUE(!strncmp(line, "Host: example.com", line_len));

	EXPECT_EQ_INT(_next_line(raw, strlen(raw), &offset, &line, &line_len), 1);
	EXPECT_EQ_INT((int)line_len, 0);

	EXPECT_EQ_INT(_next_line(raw, strlen(raw), &offset, &line, &line_len), 0);
	return (0);
}

static int	test_methods_conversion(void)
{
	EXPECT_EQ_INT(_http_str_to_method("GET"), HTTP_GET);
	EXPECT_EQ_INT(_http_str_to_method("POST"), HTTP_POST);
	EXPECT_EQ_INT(_http_str_to_method("HEAD"), HTTP_HEAD);
	EXPECT_EQ_INT(_http_str_to_method("get"), HTTP_ERROR);
	EXPECT_EQ_INT(_http_str_to_method(NULL), HTTP_ERROR);

	EXPECT_STREQ(_http_methods_str(HTTP_ERROR), "ERROR");
	EXPECT_STREQ(_http_methods_str(HTTP_GET), "GET");
	EXPECT_STREQ(_http_methods_str(HTTP_POST), "POST");
	EXPECT_STREQ(_http_methods_str(HTTP_HEAD), "HEAD");
	return (0);
}

static int	test_headers_add_find(void)
{
	t_http_header_list	list = {0};
	t_http_header		*h1;
	t_http_header		*found;

	EXPECT_EQ_INT(_http_setup_header_list(&list, 16), error_none);
	h1 = _http_new_header("Host", "example.com");
	EXPECT_NOT_NULL(h1);

	EXPECT_EQ_INT(_http_add_header(&list, h1), error_none);
	EXPECT_EQ_INT((int)list.size, 1);
	EXPECT_TRUE(list.head == h1);
	EXPECT_TRUE(list.tail == h1);

	found = _http_header_find(&list, "host");
	EXPECT_NOT_NULL(found);
	EXPECT_STREQ(found->key, "host");
	EXPECT_STREQ(found->value, "example.com");

	_http_free_list(&list, 1);
	return (0);
}

static int	test_http_to_raw_serialization(void)
{
	t_http_header_list	list = {0};
	t_http				request = {0};
	t_http_body			body = {0};
	char				buffer[256];
	char				*raw;

	EXPECT_EQ_INT(_http_setup_header_list(&list, 16), error_none);
	EXPECT_EQ_INT(http_add_header_raw(&list, "Host", "example.com"), error_none);

	body.content = "hello";
	body.capacity = 6;

	request.version = HTTP_1_0;
	request.method = HTTP_POST;
	request.headers = list;
	request.body = body;
	request.path = "/upload";
	request.content_length = 5;

	raw = http_to_raw(&request, buffer, sizeof(buffer));
	EXPECT_NOT_NULL(raw);
	EXPECT_TRUE(raw == buffer);
	EXPECT_NOT_NULL(strstr(raw, "POST /upload HTTP/1.0\r\n"));
	EXPECT_NOT_NULL(strstr(raw, "host: example.com\r\n"));
	EXPECT_NOT_NULL(strstr(raw, "Content-Length: 5\r\n"));
	EXPECT_NOT_NULL(strstr(raw, "\r\n\r\nhello"));

	raw = http_to_raw(&request, NULL, 0);
	EXPECT_NOT_NULL(raw);
	EXPECT_NOT_NULL(strstr(raw, "POST /upload HTTP/1.0\r\n"));
	mem_free(raw);

	_http_free_list(&list, 1);
	return (0);
}

static int	test_http_to_raw_small_buffer(void)
{
	t_http_header_list	list = {0};
	t_http				request = {0};
	t_http_body			body = {0};
	char				buffer[8];

	EXPECT_EQ_INT(_http_setup_header_list(&list, 16), error_none);
	EXPECT_EQ_INT(http_add_header_raw(&list, "Host", "example.com"), error_none);

	body.content = "hello";
	body.capacity = 6;

	request.version = HTTP_1_0;
	request.method = HTTP_POST;
	request.headers = list;
	request.body = body;
	request.path = "/upload";
	request.content_length = 5;

	EXPECT_NULL(http_to_raw(&request, buffer, sizeof(buffer)));

	_http_free_list(&list, 1);
	return (0);
}

static int	test_http_parse_str_valid_request(void)
{
	const char	*raw_request =
		"GET /index.html HTTP/1.0\r\n"
		"Host: example.com\r\n"
		"\r\n";
	t_http		*request = http_parse_str(raw_request);

	EXPECT_NOT_NULL(request);
	EXPECT_EQ_INT(request->method, HTTP_GET);
	EXPECT_STREQ(request->path, "/index.html");

	http_free(request);
	return (0);
}

static int	_child_http_new(void)
{
	t_http	*http = http_new();

	if (!http)
		return (1);
	http_free(http);
	return (0);
}

static int	test_http_new_no_crash(void)
{
	pid_t	pid;
	int		status;

	pid = fork();
	EXPECT_TRUE(pid >= 0);
	if (pid == 0)
		_exit(_child_http_new());

	EXPECT_EQ_INT(waitpid(pid, &status, 0), (int)pid);
	EXPECT_TRUE(WIFEXITED(status));
	EXPECT_EQ_INT(WEXITSTATUS(status), 0);
	return (0);
}

int	main(void)
{
	run_test("parse_version_valid", test_parse_version_valid);
	run_test("parse_version_invalid", test_parse_version_invalid);
	run_test("version_to_str", test_version_to_str);
	run_test("str_to_nb", test_str_to_nb);
	run_test("next_line", test_next_line);
	run_test("methods_conversion", test_methods_conversion);
	run_test("headers_add_find", test_headers_add_find);
	run_test("http_to_raw_serialization", test_http_to_raw_serialization);
	run_test("http_to_raw_small_buffer", test_http_to_raw_small_buffer);
	run_test("http_parse_str_valid_request", test_http_parse_str_valid_request);
	run_test("http_new_no_crash", test_http_new_no_crash);

	printf("\n[SUMMARY] total=%d passed=%d failed=%d\n",
		g_tests_run,
		g_tests_run - g_tests_failed,
		g_tests_failed);

	return (g_tests_failed != 0);
}
