/**
 * 
*/
/* ----| Headers    |----- */
	/* Standard */
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <limits.h>

#include <linux/limits.h>

#include <sys/wait.h>
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

int	_execute(
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

int _dir_rm(const char *const restrict path)
{
	DIR				*dir;
	struct dirent	*entry;
	struct stat		st;
	char			fullpath[PATH_MAX];
	int				err = 0;

	if (rmdir(path) == 0)
		return (0);

	if (unlikely(errno != ENOTEMPTY && errno != EEXIST))
	{
		fprintf(stderr, "cannot remove dir %s: %s\n", path, strerror(errno));
		return (-errno);
	}

	dir = opendir(path);
	if (unlikely(!dir))
	{
		fprintf(stderr, "cannot open dir %s: %s\n", path, strerror(errno));
		return (-errno);
	}

	while ((entry = readdir(dir)))
	{
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			continue;

		snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

		if (unlikely(lstat(fullpath, &st)))
		{
			fprintf(stderr, "stat failed on %s: %s\n", fullpath, strerror(errno)); 
			closedir(dir);
			return (-errno);
		}

		if (S_ISDIR(st.st_mode))
			err = _dir_rm(fullpath);
		else
			err = unlink(fullpath);

		if (unlikely(err))
		{
			fprintf(stderr, "cannot remove %s: %s\n", fullpath, strerror(errno));
			closedir(dir);
			return (-errno);
		}
	}

	closedir(dir);

	if (unlikely(rmdir(path)))
	{
		fprintf(stderr, "cannot remove dir %s: %s\n", path, strerror(errno));
		return (-errno);
	}

	return (0);
}

static int _is_allowed(
	const char *const			relpath,
	char **allowed
)
{
	size_t	i;
	size_t	len;

	i = 0;
	len = strlen(relpath);

	while (allowed[i])
	{
		if (!strcmp(relpath, allowed[i]))
			return (1);
		else if (!strncmp(relpath, allowed[i], len) && allowed[i][len] == '/')
			return (1);

		++i;
	}

	return (0);
}

__attribute_maybe_unused__
static int _iterate(
	const char *const				root,
	const char *const				relpath,
	char **		allowed
)
{
	DIR				*dir;
	struct dirent	*entry;
	struct stat		st;
	char			fullpath[PATH_MAX];
	char			childrel[PATH_MAX];
	int				err;

	dir = opendir(root);
	if (!dir)
		return (-errno);

	while ((entry = readdir(dir)))
	{
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..") || !strcmp(entry->d_name, ".git") || !strcmp(entry->d_name, "_internal_"))
			continue;

		snprintf(fullpath, sizeof(fullpath), "%s/%s",root, entry->d_name);

		if (*relpath)
			snprintf(childrel, sizeof(childrel), "%s/%s", relpath, entry->d_name);
		else
			snprintf(childrel, sizeof(childrel), "%s", entry->d_name);

		if (lstat(fullpath, &st))
		{
			closedir(dir);
			return (-errno);
		}

		if (!S_ISDIR(st.st_mode))
			continue;

		if (!_is_allowed(childrel, allowed))
		{
			err = _dir_rm(fullpath);
			if (err)
			{
				closedir(dir);
				return (err);
			}

			continue;
		}

		err = _iterate(fullpath, childrel, allowed);
		if (err)
		{
			closedir(dir);
			return (err);
		}
	}

	closedir(dir);
	return (0);
}

/* ----| Public     |----- */

int	setup(
	Config *const config
)
{
	int		err = 0;
	char	**allowed = NULL;
	int		nb_allowed = 0;

	fprintf(stderr, "url=%s, dest=%s\n", config->conf.remote_url, config->dest);	//rm
	err = _execute("git", (char *[5]){"git", "clone", config->conf.remote_url, config->dest, NULL});
	if (unlikely(err))
		return (err);

	allowed = mem_alloc(sizeof(char *) * (config->conf.modules.length + 1));
	if (unlikely(!allowed))
		return (error_alloc_fail);

	for (size_t i = 0; i < config->conf.modules.length; i++)
	{
		t_module *const	this = (t_module *const)config->conf.modules.data[i];

		allowed[nb_allowed++] = this->path;
	}

	err = _iterate(config->dest, "", allowed);
	cmake_write(config);

	return (error_none);
}
