/**
 * 
*/
/* ----| Headers    |----- */
	/* Standard */
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <limits.h>
#include <fcntl.h>

#include <linux/limits.h>

#include <sys/stat.h>

	/* Internal */
#include "manager.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

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

static char	*fs_str_dup(
	const char *const	src
)
{
	if (unlikely(!src))
		return (NULL);

	return (mem_dup(src, strlen(src) + 1));
}

static int	mkdir_checked(
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

static int	mkdir_recursive(
	const char *const	path
)
{
	char	*tmp;
	int		err;

	if (unlikely(!path || !*path))
		return (error_none);

	tmp = fs_str_dup(path);
	if (unlikely(!tmp))
		return (error_alloc_fail);

	for (size_t i = tmp[0] == '/'; tmp[i]; ++i)
	{
		if (tmp[i] != '/')
			continue ;

		tmp[i] = '\0';
		err = mkdir_checked(tmp);
		tmp[i] = '/';
		if (unlikely(err))
		{
			mem_free(tmp);
			return (err);
		}
	}

	err = mkdir_checked(tmp);
	mem_free(tmp);
	return (err);
}

static int	mkdir_parent_mode(
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

	err = mkdir_parent_mode(dest, 0777);
	if (unlikely(err < 0))
		return (err);

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

/* ----| Public     |----- */

int dir_rm(
	const char *const restrict path
)
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
			err = dir_rm(fullpath);
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

char	*path_join(
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

int	mkdir_parent(
	const char *const	path
)
{
	char	*parent;
	char	*slash;
	int		err;

	if (unlikely(!path || !*path))
		return (-EINVAL);

	parent = fs_str_dup(path);
	if (unlikely(!parent))
		return (error_alloc_fail);

	slash = strrchr(parent, '/');
	if (!slash)
	{
		mem_free(parent);
		return (error_none);
	}

	*slash = '\0';
	err = mkdir_recursive(parent);
	mem_free(parent);
	return (err);
}

int	is_dir_empty(
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

int	cache_has_git_metadata(
	const char *const	path
)
{
	struct stat	st;
	char		*git_path;
	int			result;

	git_path = path_join(path, ".git");
	if (unlikely(!git_path))
		return (0);

	result = !stat(git_path, &st);
	mem_free(git_path);
	return (result);
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
