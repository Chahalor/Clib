#include "../../log.h"
#include "../_internal_/_log.h"

#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	t_log_report	report4 = {
		.body = "some\nbody\nwith\nfifferents line",
		.code = 0,
		.file = __FILE__,
		.func = __func__,
		.level = log_debug,
		.line = __LINE__,
		.sub = NULL,
		.summary = "some test summary"
	};

	t_log_report	report3 = {
		.body = "some\nbody\nwith\nfifferents line",
		.code = 0,
		.file = __FILE__,
		.func = __func__,
		.level = log_info,
		.line = __LINE__,
		.sub = &report4,
		.summary = "some test summary"
	};

	t_log_report	report2 = {
		.body = "some\nbody\nwith\nfifferents line",
		.code = 0,
		.file = __FILE__,
		.func = __func__,
		.level = log_warning,
		.line = __LINE__,
		.sub = &report3,
		.summary = "some test summary"
	};

	t_log_report	report = {
		.body = "some\nbody\nwith\nfifferents line",
		.code = 0,
		.file = __FILE__,
		.func = __func__,
		.level = log_error,
		.line = __LINE__,
		.sub = &report2,
		.summary = "some test summary"
	};

	int fd = open(".dev/bob", O_CREAT | O_TRUNC | O_WRONLY, 0777);

	_logs_print(&report, fd, 0);
	_logs_print(&report, 1, 0);

	return 0;
}
