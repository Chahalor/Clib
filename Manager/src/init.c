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

int	config_load(
	Config *const		config,
	const char *const	path
)
{
	TOML	*file;
	int		result = error_none;

	file = toml_load_file(path);
	if (unlikely(!file))
	{
		result = -errno;
		config->consts.url_git = DEFAULT_URL_GIT;
		config->consts.path_cache_dir = DEFAULT_CACHE_DIR;
		config->consts.version.major = VERSION_MAJOR;
		config->consts.version.minor = VERSION_MINOR;
		config->consts.version.patch = VERSION_PATCH;

		return (result);
	}

	if (!config->consts.url_git)
	{
		config->consts.url_git = toml_get(file, "url");
		if (!config->consts.url_git)
		{
			config->consts.url_git = toml_get(file, "url-remote");
			if (!config->consts.url_git)
				config->consts.url_git = DEFAULT_URL_GIT;
		}
	}

	if (!config->consts.path_cache_dir)
	{
		config->consts.path_cache_dir = toml_get(file, "path-cache");
		if (!config->consts.path_cache_dir)
			config->consts.path_cache_dir = DEFAULT_CACHE_DIR;
	}

	char	*version_str = toml_get(file, "version");
	if (version_str)
	{
		char	*start = strchr(version_str, '.');
		config->consts.version.major = atoi(start);
		start = strchr(start - version_str,' .');
		config->consts.version.minor = atoi(start);
		start = strchr(start - version_str,' .');
		config->consts.version.patch = atoi(start);
	}

	toml_unload(file);
	return (error_none);
}

int	modules_load(
	Config *const		config,
	const char *const	path
)
{
	struct s_conf_file *const	f = &config->lib;

	f->toml = toml_load_file(path);
	if (unlikely(!f->toml))
	{
		toml_error_dump(stderr);
		return (toml_errno());
	}

	f->name = toml_get(f->toml, "name");
	if (!f->name)
		f->name = toml_get(f->toml, "Name");

	f->version = toml_get(f->toml, "version");
	if (!f->version)
		f->version = toml_get(f->toml, "Version");

	toml_foreach(array, f->toml)
	{
		t_module	*new = NULL;
		TOML		*node;

		if (toml_get_type(array) != toml_table)
			continue ;

		new = module_new();
		if (unlikely(!new))
			return (1);

		new->name = toml_get(array, "name");
		if (!new->name)
			new->name = array->key;

		new->path = toml_get(array, "path");
		new->public_header = toml_get(array, "publicHeaders");
		new->private_header = toml_get(array, "privateHeaders");
		node = toml_get(array, "dependencies");
		if (node && toml_len(node))
		{
			toml_foreach(dep, node)
			{
				array_append(&new->dependencies, dep);
			}
		}

		node = toml_get(array, "tags");
		if (node && toml_len(node))
		{
			toml_foreach(dep, node)
			{
				array_append(&new->dependencies, dep);
			}
		}

		node = toml_get(array, "controls");
		if (node && toml_len(node))
		{
			toml_foreach(dep, node)
			{
				array_append(&new->dependencies, dep);
			}
		}

		node = toml_get(array, "defines");
		if (node && toml_len(node))
		{
			toml_foreach(dep, node)
			{
				array_append(&new->dependencies, dep);
			}
		}

		array_append(&config->lib.modules, new);
	}

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


int	init_all(
	Config *const config
)
{
	int	err = 0;

	if (config_load(config, config->consts.path_config_file))
	{
		TOML	*config_file;
		FILE	*file;

		config_file = toml_new();
		if (unlikely(!config_file))
		{
			toml_error_dump(stderr);
			return (toml_errno());
		}

		toml_set(config_file, "name", "clib");
		toml_set(config_file, "version", "%d.%d.%d", config->consts.version.major, config->consts.version.minor, config->consts.version.patch);
		toml_set(config_file, "url-remote", config->consts.url_git);
		toml_set(config_file, "path-cache", config->consts.path_cache_dir);

		file = fopen(config->consts.path_cache_dir, "w");
		if (unlikely(!file))
		{
			fprintf(stderr, "could not open %s: %s\n", config->consts.path_cache_dir, strerror(errno));
			return (-errno);
		}

		toml_dump(config_file, file, 4);
		fclose(file);
		toml_unload(config_file);
	}

	err = init_cache(config);

	return (err);
}