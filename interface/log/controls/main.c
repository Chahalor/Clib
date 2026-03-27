#include "../log.h"
#include "../_internal_/_log.h"

int main(void)
{
	t_log_report	report = {
		.body = "some\nbody\nwith\nfifferents line",
		.code = 0,
		.file = __FILE__,
		.func = __func__,
		.level = log_error,
		.line = __LINE__,
		.sub = NULL,
		.summary = "some test summary"
	};

	_logs_print(&report, 1, 0);

	return 0;
}
