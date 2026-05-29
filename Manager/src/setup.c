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
#include <fcntl.h>

#include <linux/limits.h>

#include <sys/wait.h>
#include <sys/stat.h>

	/* Internal */
#include "types.h"
#include "manager.h"

	/* External */
#include "files/toml.h"
#include "interface/args.h"
#include "standards/formating.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

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

int mkdir_p(
	const char *	path,
	mode_t			mode
)
{
	char	tmp[PATH_MAX];
	char	*p;
	int		nb_creation = 0;

	if (unlikely(strlen(path) >= sizeof(tmp)))
		return (-ENAMETOOLONG);

	strcpy(tmp, path);

	for (p = tmp + 1; *p; p++)
	{
		if (*p == '/')
		{
			*p = '\0';

			if (unlikely(mkdir(tmp, mode) < 0 && errno != EEXIST))
				return (-errno);

			nb_creation++;
			*p = '/';
		}
	}

	if (unlikely(mkdir(tmp, mode) < 0 && errno != EEXIST))
		return (-errno);

	return (nb_creation++);
}

#define GOTO_ERROR() \
	do \
	{ \
		result = -errno; \
		goto error; \
	} while (0)

static int	copy_file(
	const char *const	src,
	const char *const	dst
)
{
	int			in, out, err;
	ssize_t		n, w;
	char		buf[8192];
	char		*p;
	struct stat	st;

	if (unlikely(stat(src, &st) < 0))
		return (-errno);

	in = open(src, O_RDONLY);
	if (unlikely(in < 0))
		return (-errno);

	out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, st.st_mode & 0777);
	if (unlikely(out < 0))
	{
		err = errno;
		close(in);
		return (-err);
	}

	while (true)
	{
		do
			n = read(in, buf, sizeof(buf));
		while (n < 0 && errno == EINTR);

		if (n <= 0)
			break;

		p = buf;

		while (n > 0)
		{
			do
				w = write(out, p, n);
			while (w < 0 && errno == EINTR);

			if (unlikely(w < 0))
			{
				err = errno;
				close(in);
				close(out);
				return (-err);
			}

			p += w;
			n -= w;
		}
	}

	if (n < 0)
	{
		err = errno;
		close(in);
		close(out);
		return (-err);
	}

	close(in);
	close(out);
	return (0);
}

static int	mkdir_parent(
	const char *const	path,
	const mode_t		mode
)
{
	char	parent[PATH_MAX];
	char	*slash;

	if (unlikely(!path || snprintf(parent, sizeof(parent), "%s", path) >= (int)sizeof(parent)))
		return (-ENAMETOOLONG);

	slash = strrchr(parent, '/');
	if (!slash)
		return (error_none);

	*slash = '\0';
	if (!*parent)
		return (error_none);

	return (mkdir_p(parent, mode));
}

int	copy_dir(
	const char *const	src,
	const char *const	dst
)
{
	int				result;
	DIR				*dir;
	struct stat		st;
	struct dirent	*entry;
	char			src_path[PATH_MAX];
	char			dst_path[PATH_MAX];

	result = error_none;

	dir = opendir(src);
	if (unlikely(!dir))
		return (-errno);

	if (unlikely(lstat(src, &st) < 0))
		GOTO_ERROR();

	result = mkdir_p(dst, st.st_mode & 0777);
	if (unlikely(result < 0))
		GOTO_ERROR();

	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;

		if (snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name) >= (int)sizeof(src_path))
		{
			result = -ENAMETOOLONG;
			goto error;
		}

		if (snprintf(dst_path, sizeof(dst_path), "%s/%s", dst, entry->d_name ) >= (int)sizeof(dst_path))
		{
			result = -ENAMETOOLONG;
			goto error;
		}

		if (unlikely(lstat(src_path, &st) < 0))
			GOTO_ERROR();

		if (S_ISDIR(st.st_mode))
		{
			result = copy_dir(src_path, dst_path);

			if (unlikely(result < 0))
				goto error;
		}
		else if (S_ISREG(st.st_mode))
		{
			result = copy_file(src_path, dst_path);

			if (unlikely(result < 0))
				goto error;
		}
		else if (S_ISLNK(st.st_mode))
		{
			char	link_target[PATH_MAX];
			ssize_t	len;

			len = readlink(src_path, link_target, sizeof(link_target) - 1 );

			if (unlikely(len < 0))
				GOTO_ERROR();

			link_target[len] = '\0';

			if (unlikely(symlink(link_target, dst_path) < 0))
			{
				if (errno != EEXIST)
					GOTO_ERROR();
			}
		}
	}

error:
	closedir(dir);
	return (result);
}

static t_module	*find_module(
	const Config *const		config,
	const char *const		name
)
{
	for (size_t i = 0; i < config->lib.modules.length; i++)
	{
		t_module	*const	this = config->lib.modules.data[i];

		if (this->name && !strcmp(name, this->name))
			return (this);
	}
	return (NULL);
}

static int	module_is_selected(
	const t_array *const	modules,
	const t_module *const	module
)
{
	for (size_t i = 0; i < modules->length; i++)
	{
		if (modules->data[i] == module)
			return (true);
	}
	return (false);
}

static int	resolve_module(
	const Config *const	config,
	t_array *const		modules,
	const char *const	name
)
{
	t_module	*module;
	int			err;

	if (unlikely(!name || !*name))
		return (-EINVAL);

	module = find_module(config, name);
	if (unlikely(!module))
	{
		fprintf(stderr, "[" YELLOW "warn" RESET "] module " BOLD "%s" RESET " not found in the clib config\n", name);
		return (-ENOENT);
	}

	if (module_is_selected(modules, module))
		return (error_none);

	for (size_t i = 0; i < module->dependencies.length; i++)
	{
		TOML *const	dep = module->dependencies.data[i];

		if (unlikely(!dep || !dep->data))
			return (-EINVAL);

		err = resolve_module(config, modules, dep->data);
		if (unlikely(err))
			return (err);
	}

	return (array_append(modules, module));
}

static int	copy_optional_file(
	const Config *const	config,
	const char *const	relative_path
)
{
	char	src[PATH_MAX];
	char	dest[PATH_MAX];
	int		err;

	if (!relative_path || !*relative_path)
		return (error_none);

	snprintf(src, sizeof(src), "%s/%s", config->consts.path_cache_dir, relative_path);
	snprintf(dest, sizeof(dest), "%s/%s", config->dest, relative_path);

	err = mkdir_parent(dest, 0777);
	if (unlikely(err < 0))
		return (err);

	printf("copy FILE %s -> %s\n", src, dest);
	return (copy_file(src, dest));
}

static int	write_lib_header(
	const Config *const	config,
	const t_array *const	modules
)
{
	char	path[PATH_MAX];
	FILE	*file;

	if (unlikely(snprintf(path, sizeof(path), "%s/lib.h", config->dest) >= (int)sizeof(path)))
		return (-ENAMETOOLONG);

	file = fopen(path, "w");
	if (unlikely(!file))
		return (-errno);

	fprintf(file,
		"#ifndef LIB_H\n"
		"# define LIB_H\n"
		"\n"
		"# pragma once\n"
		"\n"
	);

	for (size_t i = 0; i < modules->length; i++)
	{
		t_module *const	module = modules->data[i];

		if (module->public_header && *module->public_header)
			fprintf(file, "# include \"%s\"\n", module->public_header);
	}

	fprintf(file,
		"\n"
		"#endif\t// LIB_H\n"
	);
	fclose(file);
	return (error_none);
}

int	copy_modules(
	const Config *const	config,
	const t_array *const	modules
)
{
	char	src[PATH_MAX];
	char	dest[PATH_MAX];
	struct stat	st;
	int		err = 0;

	for (size_t i = 0; i < modules->length; i++)
	{
		t_module *const	module = modules->data[i];

		snprintf(src, sizeof(src), "%s/%s", config->consts.path_cache_dir, module->path);
		snprintf(dest, sizeof(dest), "%s/%s", config->dest, module->path);

		if (!stat(src, &st) && S_ISDIR(st.st_mode))
		{
			printf("copy DIR %s -> %s\n", src, dest);
			err = copy_dir(src, dest);
			if (unlikely(err))
			{
				perror("copy dir");
				return (err);
			}
		}
		else if (errno != ENOENT)
			return (-errno);

		err = copy_optional_file(config, module->public_header);
		if (unlikely(err))
		{
			perror("copy file");
			return (err);
		}
	}

	return (write_lib_header(config, modules));
}


/* ----| Public     |----- */

int	setup_setup(
	Config *const			config,
	t_args_output *const	output
)
{
	t_args_output_parser	*sub = args_get_sub_output(output);
	size_t					n;
	void					*_allowed = args_get_param(sub, "target", &n);

	config->cli.init = args_output_parser_get_option(sub, "init") != NULL;
	config->nb_allowed = n;
	config->allowed = mem_alloc(sizeof(char *) * (n + 1));
	if (!config->allowed)
	{
		perror("allowed alloc");
		return (errno);
	}

	if (n != 1)
		for (size_t i = 0; i < n; i++)
			config->allowed[i] = ((char **)_allowed)[i];
	else
		config->allowed[0] = (char *)_allowed;

	return (error_none);
}

int	setup(
	Config *const config
)
{
	t_array	modules;
	int		err;

	err = array_alloc(&modules, ALLOC_SIZE);
	if (unlikely(err))
		return (err);

	for (size_t i = 0; i < config->nb_allowed; i++)
	{
		const char *const	name = config->allowed[i];

		err = resolve_module(config, &modules, name);
		if (unlikely(err))
		{
			array_free(&modules, false);
			return (err);
		}
	}

	err = copy_modules(config, &modules);
	if (unlikely(err))
	{
		array_free(&modules, false);
		return (err);
	}

	err = cmake_write(config, &modules);
	if (unlikely(err))
	{
		array_free(&modules, false);
		return (err);
	}

	array_free(&modules, false);
	return (error_none);
}
