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
#include <stdio.h>
#include <dirent.h>
#include <limits.h>
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
	if (unlikely(!src))
		return (NULL);

	return (mem_dup(src, strlen(src)));
}

static char	*_path_join(
	const char *const	left,
	const char *const	right
)
{
	char	*result;
	size_t	left_len;
	size_t	right_len;
	size_t	need_slash;

	if (unlikely(!left || !*left || !right || !*right))
		return (NULL);

	left_len = strlen(left);
	right_len = strlen(right);
	need_slash = left[left_len - 1] != '/';
	result = mem_alloc(left_len + need_slash + right_len + 1);
	if (unlikely(!result))
		return (NULL);

	memcpy(result, left, left_len);
	if (need_slash)
		result[left_len++] = '/';

	memcpy(result + left_len, right, right_len + 1);
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
		dir = _path_join(base, DEFAULT_CONFIG_DIR);
	else
	{
		base = getenv("HOME");
		if (unlikely(!base || !*base))
			return (-ENOENT);

		dir = _path_join(base, ".config/" DEFAULT_CONFIG_DIR);
	}

	if (unlikely(!dir))
		return (error_alloc_fail);

	config->consts.path_config_file = _path_join(dir, DEFAULT_CONFIG_FILE);
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
		config->consts.path_cache_dir = _path_join(base, DEFAULT_CACHE_DIR);
	else
	{
		base = getenv("HOME");
		if (unlikely(!base || !*base))
			return (-ENOENT);

		config->consts.path_cache_dir = _path_join(base, ".cache/" DEFAULT_CACHE_DIR);
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

	config->consts.path_modules_file = _path_join(config->consts.path_cache_dir, DEFAULT_MODULES_FILE);
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

static int	_mkdir_checked(
	const char *const	path
)
{
	struct stat	st;

	if (!mkdir(path, 0700))
		return (error_none);

	if (errno != EEXIST)
		return (-errno);

	if (unlikely(stat(path, &st) || !S_ISDIR(st.st_mode)))
		return (-ENOTDIR);

	return (error_none);
}

static int	_mkdir_recursive(
	const char *const	path
)
{
	char	*tmp;
	int		err;

	if (unlikely(!path || !*path))
		return (error_none);

	tmp = _str_dup(path);
	if (unlikely(!tmp))
		return (error_alloc_fail);

	for (size_t i = tmp[0] == '/'; tmp[i]; ++i)
	{
		if (tmp[i] != '/')
			continue ;

		tmp[i] = '\0';
		err = _mkdir_checked(tmp);
		tmp[i] = '/';
		if (unlikely(err))
		{
			mem_free(tmp);
			return (err);
		}
	}

	err = _mkdir_checked(tmp);
	mem_free(tmp);
	return (err);
}

static int	_mkdir_parent(
	const char *const	path
)
{
	char	*parent;
	char	*slash;
	int		err;

	if (unlikely(!path || !*path))
		return (-EINVAL);

	parent = _str_dup(path);
	if (unlikely(!parent))
		return (error_alloc_fail);

	slash = strrchr(parent, '/');
	if (!slash)
	{
		mem_free(parent);
		return (error_none);
	}

	*slash = '\0';
	err = _mkdir_recursive(parent);
	mem_free(parent);
	return (err);
}

static int	_is_dir_empty(
	const char *const	path
)
{
	DIR				*dir;
	struct dirent	*entry;

	dir = opendir(path);
	if (unlikely(!dir))
		return (-errno);

	while ((entry = readdir(dir)))
	{
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			continue ;

		closedir(dir);
		return (0);
	}

	closedir(dir);
	return (1);
}

static int	_cache_has_git_metadata(
	const char *const	path
)
{
	struct stat	st;
	char		*git_path;
	int			result;

	git_path = _path_join(path, ".git");
	if (unlikely(!git_path))
		return (0);

	result = !stat(git_path, &st);
	mem_free(git_path);
	return (result);
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
	const char *const	path
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
	file = toml_load_file(path ? path : config->consts.path_config_file);
	if (unlikely(!file))
	{
		result = errno ? -errno : -ENOENT;
		errno = 0;
		defaults_err = _set_config_defaults(config);
		if (unlikely(defaults_err))
			return (defaults_err);

		if (unlikely(config->cli.verbose))
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
				array_append(&new->dependencies, dep);
		}

		node = toml_get(array, "tags");
		if (node && toml_len(node))
		{
			toml_foreach(dep, node)
				array_append(&new->dependencies, dep);
		}

		node = toml_get(array, "controls");
		if (node && toml_len(node))
		{
			toml_foreach(dep, node)
				array_append(&new->dependencies, dep);
		}

		node = toml_get(array, "defines");
		if (node && toml_len(node))
		{
			toml_foreach(dep, node)
				array_append(&new->dependencies, dep);
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

	if (!stat(config->consts.path_cache_dir, &st))
	{
		if (unlikely(!S_ISDIR(st.st_mode)))
		{
			fprintf(stderr, "cache path exists and is not a directory: %s\n", config->consts.path_cache_dir);
			return (-ENOTDIR);
		}

		if (_cache_has_git_metadata(config->consts.path_cache_dir))
		{
			if (config->cli.verbose)
				printf("Cache already initialized at %s\n", config->consts.path_cache_dir);
			return (error_none);
		}

		empty = _is_dir_empty(config->consts.path_cache_dir);
		if (unlikely(empty < 0))
			return (empty);
		if (unlikely(!empty))
		{
			fprintf(stderr, "cache directory is not empty and is not a git repo: %s\n", config->consts.path_cache_dir);
			return (-EEXIST);
		}
	}
	else if (errno == ENOENT)
	{
		err = _mkdir_parent(config->consts.path_cache_dir);
		if (unlikely(err))
			return (err);
	}
	else
		return (-errno);

	return (execute("git", (char *[5]){"git", "clone", config->consts.url_git, config->consts.path_cache_dir}));
}

int	init_all(
	Config *const config
)
{
	int	err = 0;

	err = config_load(config, config->consts.path_config_file);
	if (err && err != -ENOENT)
		return (err);

	if (err)
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

		mkdir_err = _mkdir_parent(config->consts.path_config_file);
		if (unlikely(mkdir_err))
		{
			toml_unload(config_file);
			return (mkdir_err);
		}

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
	}

	err = init_cache(config);

	return (err);
}
