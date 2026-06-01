/**
 * 
*/
/* ----| Headers    |----- */
	/* Standard */
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>

	/* Internal */
#include "types.h"
#include "manager.h"

	/* External */
#include "files/toml.h"
#include "interface/args.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static char	*_str_dup(
	const char *const	src
)
{
	char	*result;
	size_t	len;

	if (unlikely(!src))
		return (NULL);

	len = strlen(src);
	result = mem_alloc(len + 1);
	if (unlikely(!result))
		return (NULL);

	memcpy(result, src, len + 1);
	return (result);
}

static int	_set_default_config_path(
	Config *const	config
)
{
	const char	*base;
	char		*dir;

	if (config->consts.path_config_file)
		return (error_none);

	base = getenv("XDG_CONFIG_HOME");
	if (base && *base)
		dir = path_join(base, DEFAULT_CONFIG_DIR);
	else
	{
		base = getenv("HOME");
		if (unlikely(!base || !*base))
			return (-ENOENT);

		dir = path_join(base, ".config/" DEFAULT_CONFIG_DIR);
	}

	if (unlikely(!dir))
		return (error_alloc_fail);

	config->consts.path_config_file = path_join(dir, DEFAULT_CONFIG_FILE);
	mem_free(dir);
	if (unlikely(!config->consts.path_config_file))
		return (error_alloc_fail);

	config->config_file = config->consts.path_config_file;
	return (error_none);
}

static int	_set_default_cache_path(
	Config *const	config
)
{
	const char	*base;

	if (config->consts.path_cache_dir)
		return (error_none);

	base = getenv("XDG_CACHE_HOME");
	if (base && *base)
		config->consts.path_cache_dir = path_join(base, DEFAULT_CACHE_DIR);
	else
	{
		base = getenv("HOME");
		if (unlikely(!base || !*base))
			return (-ENOENT);

		config->consts.path_cache_dir = path_join(base, DEFAULT_CACHE_DIR);
	}

	if (unlikely(!config->consts.path_cache_dir))
		return (error_alloc_fail);

	return (error_none);
}

static int	_set_default_modules_path(
	Config *const	config
)
{
	if (config->consts.path_modules_file)
		return (error_none);

	config->consts.path_modules_file = path_join(config->consts.path_cache_dir, DEFAULT_MODULES_FILE);
	if (unlikely(!config->consts.path_modules_file))
		return (error_alloc_fail);

	return (error_none);
}

static int	_set_config_defaults(
	Config *const	config
)
{
	int	err;

	if (!config->consts.version.major && !config->consts.version.minor
		&& !config->consts.version.patch)
	{
		config->consts.version.major = VERSION_MAJOR;
		config->consts.version.minor = VERSION_MINOR;
		config->consts.version.patch = VERSION_PATCH;
	}

	err = _set_default_config_path(config);
	if (unlikely(err))
		return (err);

	err = _set_default_cache_path(config);
	if (unlikely(err))
		return (err);

	err = _set_default_modules_path(config);
	if (unlikely(err))
		return (err);

	if (!config->consts.url_git)
		config->consts.url_git = DEFAULT_URL_GIT;

	return (error_none);
}

static void	_init_verbose_paths(
	const Config *const	config
)
{
	if (!config->cli.verbose)
		return ;

	printf("Init configuration:\n");
	printf(" - config file  = '%s'\n", config->consts.path_config_file);
	printf(" - cache dir    = '%s'\n", config->consts.path_cache_dir);
	printf(" - modules file = '%s'\n", config->consts.path_modules_file);
	printf(" - remote url   = '%s'\n", config->consts.url_git);
	printf(" - version      = v%d.%d.%d\n", config->consts.version.major,
		config->consts.version.minor, config->consts.version.patch);
}

static int	_write_config(
	Config *const	config
)
{
	TOML	*config_file;
	FILE	*file;
	int		mkdir_err;

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

	if (unlikely((config->cli.verbose)))
		printf("Creating config parent directories\n");

	mkdir_err = mkdir_parent(config->consts.path_config_file);
	if (unlikely(mkdir_err))
	{
		toml_unload(config_file);
		return (mkdir_err);
	}

	if (unlikely((config->cli.verbose)))
		printf("Writing config file: %s\n", config->consts.path_config_file);

	file = fopen(config->consts.path_config_file, "w");
	if (unlikely(!file))
	{
		fprintf(stderr, "could not open %s: %s\n",
			config->consts.path_config_file, strerror(errno));
		toml_unload(config_file);
		return (-errno);
	}

	toml_dump(config_file, file, 4);
	fclose(file);
	toml_unload(config_file);

	if (unlikely((config->cli.verbose)))
		printf("Config file created\n");

	return (error_none);
}

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
	const char *const	path,
	const int			should_init
)
{
	TOML	*file;
	int		result = error_none;
	int		defaults_err;
	char	*version_str;

	result = _set_default_config_path(config);
	if (unlikely(result))
		return (result);

	errno = 0;
	file = toml_load_file(path);
	if (unlikely(!file))
	{
		if (toml_errno())
		{
			if (errno == ENOENT || toml_errno() == TOML_ERROR_INVALID_FILE || !should_init)
			{
				printf("It look like the Clib is not init in this setup\n");
				printf("To init, run: `modulesmng init`");
				return (-ENOENT);
			}
			else if (!should_init)
			{
				toml_error_dump(stderr);
				return (toml_errno());
			}
		}

		defaults_err = _set_config_defaults(config);
		if (unlikely(defaults_err))
			return (defaults_err);

		if (unlikely(_write_config(config)))
			return (1);

		if (unlikely((config->cli.verbose)))
		{
			printf("No default config file found, writing a new one at %s\n", config->consts.path_config_file);
			printf("Defaults values:\n");
			printf(" - remote-url = '%s'\n", config->consts.url_git);
			printf(" - dir-cache  = '%s'\n", config->consts.path_cache_dir);
			printf(" - config     = '%s'\n", config->consts.path_config_file);
			printf(" - version    = v%s\n", VERSION);
		}

		return (result);
	}

	if (unlikely((config->cli.verbose)))
		printf("Loaded config file: %s\n", path ? path : config->consts.path_config_file);

	if (!config->consts.url_git)
	{
		config->consts.url_git = _str_dup(toml_get(file, "url"));
		if (!config->consts.url_git)
		{
			config->consts.url_git = _str_dup(toml_get(file, "url-remote"));
			if (!config->consts.url_git)
				config->consts.url_git = DEFAULT_URL_GIT;
		}
	}

	if (!config->consts.path_cache_dir)
		config->consts.path_cache_dir = _str_dup(toml_get(file, "path-cache"));

	result = _set_config_defaults(config);
	if (unlikely(result))
	{
		toml_unload(file);
		return (result);
	}

	version_str = toml_get(file, "version");
	if (version_str)
		sscanf(version_str, "%d.%d.%d", &config->consts.version.major,
			&config->consts.version.minor, &config->consts.version.patch);

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

	f->remote_url = toml_get(f->toml, "url");
	if (!f->remote_url)
		f->remote_url = toml_get(f->toml, "url-remote");

	toml_foreach(array, f->toml)
	{
		t_module	*new = NULL;
		TOML		*node;
		char		*path;

		if (toml_get_type(array) != toml_table)
			continue ;

		path = toml_get(array, "path");
		if (!path)
			continue ;

		new = module_new();
		if (unlikely(!new))
			return (1);

		new->name = toml_get(array, "name");
		if (!new->name)
			new->name = array->key;

		new->path = path;
		new->public_header = toml_get(array, "publicHeaders");
		new->private_header = toml_get(array, "privateHeaders");
		node = toml_get(array, "dependencies");
		if (node && toml_len(node))
		{
			toml_foreach(dep, node)
				array_append(&new->dependencies, dep);
		}

		node = toml_get(array, "tags");
		if (node && toml_len(node))
		{
			toml_foreach(dep, node)
				array_append(&new->tags, dep);
		}

		node = toml_get(array, "controls");
		if (node && toml_len(node))
		{
			toml_foreach(dep, node)
				array_append(&new->controls, dep);
		}

		node = toml_get(array, "defines");
		if (node && toml_len(node))
		{
			toml_foreach(dep, node)
				array_append(&new->defines, dep);
		}

		array_append(&config->lib.modules, new);
	}

	return (error_none);
}

int	init_cache(
	const Config *const config
)
{
	struct stat	st;
	int			empty;
	int			err;

	if (unlikely(!config->consts.url_git || !*config->consts.url_git))
	{
		fprintf(stderr, "missing cache remote url\n");
		return (-EINVAL);
	}

	if (unlikely((config->cli.verbose)))
		printf("Checking cache directory: %s\n", config->consts.path_cache_dir);

	if (!stat(config->consts.path_cache_dir, &st))
	{
		if (unlikely((config->cli.verbose)))
			printf("Cache path exists\n");

		if (unlikely(!S_ISDIR(st.st_mode)))
		{
			fprintf(stderr, "cache path exists and is not a directory: %s\n", config->consts.path_cache_dir);
			return (-ENOTDIR);
		}

		if (cache_has_git_metadata(config->consts.path_cache_dir))
		{
			if (unlikely((config->cli.verbose)))
				printf("Cache already initialized at %s\n", config->consts.path_cache_dir);
			return (error_none);
		}

		empty = is_dir_empty(config->consts.path_cache_dir);
		if (unlikely(empty < 0))
			return (empty);
		if (unlikely(!empty))
		{
			fprintf(stderr, "cache directory is not empty and is not a git repo: %s\n", config->consts.path_cache_dir);
			return (-EEXIST);
		}

		if (unlikely((config->cli.verbose)))
			printf("Cache directory is empty, cloning into it\n");
	}
	else if (errno == ENOENT)
	{
		if (unlikely((config->cli.verbose)))
			printf("Cache directory does not exist, creating parent directories\n");

		err = mkdir_parent(config->consts.path_cache_dir);
		if (unlikely(err))
			return (err);
	}
	else
		return (-errno);

	if (unlikely((config->cli.verbose)))
	{
		printf("Cloning cache from '%s' into '%s'\n",
			config->consts.url_git, config->consts.path_cache_dir);
		fflush(stdout);
	}

	return (execute("git", (char *[5]){"git", "clone", config->consts.url_git, config->consts.path_cache_dir}));
}

int	init_all(
	Config *const config
)
{
	int	err = 0;

	if (unlikely((config->cli.verbose)))
		printf("Initializing clib manager session\n");

	err = config_load(config, config->consts.path_config_file, true);
	if (err && err != -ENOENT)
		return (err);

	_init_verbose_paths(config);

	if (err)
	{
		err = _write_config(config);
		if (unlikely(err))
			return (err);
	}
	else if (unlikely((config->cli.verbose)))
		printf("Config file already exists, keeping current settings\n");

	err = init_cache(config);
	if (!err && config->cli.verbose)
		printf("Init completed\n");

	return (err);
}
