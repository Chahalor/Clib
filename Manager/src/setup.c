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

	if (strlen(path) >= sizeof(tmp))
		return (-ENAMETOOLONG);

	strcpy(tmp, path);

	for (p = tmp + 1; *p; p++)
	{
		if (*p == '/')
		{
			*p = '\0';

			if (mkdir(tmp, mode) < 0 && errno != EEXIST)
				return (-errno);

			*p = '/';
		}
	}

	if (mkdir(tmp, mode) < 0 && errno != EEXIST)
		return (-errno);

	return (0);
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
	const Config *const		config,
	const t_module *const	allowed,
	const int				nb_allowed
)
{
	char	src[PATH_MAX];
	char	dest[PATH_MAX];
	int		err = 0;

	for (int i = 0; i < nb_allowed; i++)
	{
		snprintf(src, sizeof(src), "%s/%s", config->consts.path_cache_dir, allowed[i].path);
		snprintf(dest, sizeof(src), "%s/%s", config->dest, allowed[i].path);

		printf("copy DIR %s -> %s\n", src, dest);
		err = copy_dir(src, dest);
		if (unlikely(err))
		{
			perror("copy dir");
			return (err);
		}

		snprintf(src, sizeof(src), "%s/%s", config->consts.path_cache_dir, allowed[i].public_header);
		snprintf(dest, sizeof(src), "%s/%s", config->dest, allowed[i].public_header);

		printf("copy FILE %s -> %s\n", src, dest);
		err = copy_file(src, dest);
		if (unlikely(err))
		{
			perror("copy file");
			return (err);
		}
	}

	return (err);
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
	t_module	*allowed;
	int			nb_allowed = 0;

	allowed = mem_alloc(sizeof(t_module) * (config->nb_allowed + 1));
	if (unlikely(!allowed))
		return (error_alloc_fail);

	for (size_t i = 0; i < config->nb_allowed; i++)
	{
		const char *const	name = config->allowed[i];
		int					found = false;

		for (size_t j = 0; j < config->lib.modules.length; j++)
		{
			t_module	*const this = config->lib.modules.data[j];

			if (strcmp(name, this->name))
				continue ;

			allowed[nb_allowed++] = *this;
			found = true;
			break ;
		}

		if (unlikely(!found))
			fprintf(stderr, "[" YELLOW "warn" RESET "] module " BOLD "%s" RESET " not found in the clib config\n", name);
	}

	copy_modules(config, allowed, nb_allowed);
	cmake_write(config);

	return (error_none);
}
