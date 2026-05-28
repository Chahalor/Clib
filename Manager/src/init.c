/**
 * 
*/
/* ----| Headers    |----- */
	/* Standard */
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <wordexp.h>
#include <sys/stat.h>

	/* Internal */
#include "types.h"
#include "manager.h"

	/* External */
#include "files/toml.h"
#include "interface/args.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

/* ----| Publics    |----- */

int	execute(
	const char *const	command,
	char *const *		argv
)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (unlikely(pid == -1))
	{
		perror("fork");
		return (-errno);
	}
	else if (pid == 0)
	{
		execvp(command, argv);

		perror("execvp");
		_exit(errno ? errno : EXIT_FAILURE);
	}

	if (waitpid(pid, &status, 0) == -1)
	{
		perror("waitpid");
		return (-errno);
	}

	if (WIFEXITED(status))
		return (-WEXITSTATUS(status));

	else if (WIFSIGNALED(status))
		return (-128 - WTERMSIG(status));

	return (-ECHILD);
}

int	init_config(
	Config *const		config,
	const char *const	path
)
{
	TOML	*file;

	file = toml_load_file(path);
	if (unlikely(!file))
	{
		toml_error_dump(stderr);
		return (toml_errno());
	}

	config->consts.url_git = toml_get(file, "url");
	if (!config->consts.url_git)
	{
		config->consts.url_git = toml_get(file, "url-remote");
		if (!config->consts.url_git)
			config->consts.url_git = DEFAULT_URL_GIT;
	}

	config->consts.path_cache_dir = toml_get(file, "path-cache");
	if (!config->consts.path_cache_dir)
		config->consts.path_cache_dir = DEFAULT_CACHE_DIR;

	return (error_none);
}

int	init_cache(
	const Config *const config
)
{
	/**
	 * before check if the repo is empty or if this is a file
	*/
	return (execute("git", (char *[5]){"git", "clone", config->consts.url_git, config->consts.path_cache_dir}));
}
