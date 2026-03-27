// Header

/* ----| Headers    |----- */
	/* Standard */
#include <fcntl.h>
#include <unistd.h>

	/* Internal */
#include "_log.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static int	_setup_log_file(
	const t_log_file	*const file,
	int					*const fd
)
{
	if (file->filename)
		(*fd) = open(file->filename, O_CREAT | O_APPEND | O_RDWR, 0644);
	else
		*fd = STDERR_FILENO;

	return (error_none);
}

/* ----| Public     |----- */

int	_log_init(
	t_log_internal		*const config,
	const t_log_init	*const data
)
{
	if (unlikely(!data || !config))
		return (error_invalid_arg);

	_setup_log_file(&data->error, &config->fd[log_error]);
	_setup_log_file(&data->warning, &config->fd[log_warning]);
	_setup_log_file(&data->info, &config->fd[log_info]);
	_setup_log_file(&data->debug, &config->fd[log_debug]);
	_setup_log_file(&data->other, &config->fd[log_other]);

	config->is_init = 1;

	return (error_none);
}