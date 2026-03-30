/**
 * @file main.c
 * @brief Manual control/example entry point for `interface/log`.
 *
 * This file demonstrates how to initialize log streams and submit both raw and
 * report-based entries.
 */

#include "../../log.h"
#include "../_internal_/_log.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/**
 * @brief Local helper used by this control binary to initialize log files.
 *
 * @return Forwarded result from `log_init(...)`.
 */
int	sentinel_setup_logs(void)
{
	t_log_init	_init = {
		.debug = {
			.filename = ".dev/debug.log",
		},
		.warning = {
			.filename = ".dev/warning.log",
		},
		.info = {
			.filename = ".dev/info.log",
		},
		.error = {
			.filename = ".dev/error.log",
		},
		.other = {
			.filename = ".dev/other.log",
		},
		.display_limit = log_other
	};

	return (log_init(&_init));
}

/**
 * @brief Example program that emits several log forms.
 *
 * @return Process status code (`0`).
 */
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

	sentinel_setup_logs();

	t_log_report	*r = &report;
	logs(r);
	logs(log_warning, 0, "test with raw", "body\nsecond line");
	logs_perror(&report2);
	logs_perror(log_debug, "perror in raw", "more body\nmore new lines\n");
	// logs(&report4);

	return 0;
}
