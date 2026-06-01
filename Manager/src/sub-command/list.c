/**
 * 
*/
/* ----| Headers    |----- */
	/* Standard */
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include <linux/limits.h>

	/* Internal */
#include "types.h"
#include "manager.h"

	/* External */
#include "interface/args.h"
#include "standards/formating.h"

/* ----| Prototypes |----- */

typedef struct s_size_unit
{
	const char	*name;
	uint64_t	divisor;
}	t_size_unit;

struct s_mod
{
	t_size_unit	unit;
	char		*name;
	char		*version;
	char		**deps;
	size_t		size;
	int			nb_deps;
	int			nb_files;
};

typedef struct s_display
{
	int	max_name;
	int	max_version;
	int	max_deps;
	int	max_files;
	int	max_size;
}	t_display;

#define	FORMAT_MODULE	" - %s"
#define	FORMAT_VERSION	" (%s)"
/* Deps Files Size unit(of size) */
#define	FORMAT_STATS	" %d %d %zu%s"

/* ----| Internals  |----- */

static t_size_unit	get_unit(
	const uint64_t	size
)
{
	if (size >= 1024ULL * 1024 * 1024 * 1024)
		return ((t_size_unit){
			.name = "TiB",
			.divisor = 1024ULL * 1024 * 1024 * 1024
		});

	if (size >= 1024ULL * 1024 * 1024)
		return ((t_size_unit){
			.name = "GiB",
			.divisor = 1024ULL * 1024 * 1024
		});

	if (size >= 1024ULL * 1024)
		return ((t_size_unit){
			.name = "MiB",
			.divisor = 1024ULL * 1024
		});

	if (size >= 1024ULL)
		return ((t_size_unit){
			.name = "KiB",
			.divisor = 1024ULL
		});

	return ((t_size_unit){
		.name = "B",
		.divisor = 1
	});
}

// static inline int	get_max_size(
// 	const t_array *const modules
// )
// {
// 	int	result = 0;

// 	for (size_t i = 0; i < modules->length; i++)
// 	{
// 		const int length = strlen((char *)modules->data[i]);

// 		if (length > result)
// 			result = length;
// 	}

// 	return (result);
// }

static int	_parse_all(
	const Config *const	config,
	t_array *const		array,
	t_display *const	display
)
{
	for (size_t i = 0; i < config->lib.modules.length; i++)
	{
		const t_module	*const	this = config->lib.modules.data[i];
		// const int				nb_deps = this->dependencies.length;
		struct s_mod			*module = NULL;


		module = mem_alloc(sizeof(*module) + sizeof(char *) * this->dependencies.length);
		if (unlikely(!module))
		{
			perror("mem_alloc");
			return (-errno);
		}
		module->deps = (char **)(module + 1);
		module->name = this->name;
		module->nb_deps = this->dependencies.length;

		if (config->sub.list.options.version)
		{
			char *		version = config->sub.list.options.version
									? toml_get(config->lib.toml, "%s.version", this->name)
									: "";
			const int	length = strlen(version);

			module->version = version;
			if (length > display->max_version)
				display->max_version = length;
		}

		if (config->sub.list.options.stats)
		{
			char					path[PATH_MAX];
			snprintf(path, sizeof(path), "%s/%s", config->consts.path_cache_dir, this->path);
			const int			nb_files = count_dir_files(path, true);
			const size_t		size = dir_size(path);
			const t_size_unit	unit = get_unit(size);

			module->nb_files = nb_files;
			module->unit = unit;
			module->size = size;
			// TODO check for the display size
		}

		if (config->sub.list.options.deps && this->dependencies.length)
		{
			for (size_t j = 0; j < this->dependencies.length; j++)
			{
				const TOML	*const	dep = this->dependencies.data[j];

				module->deps[j] = dep->data;
				// TODO: check for the display size of each deps
			}
		}

		array_append(array, (void *)module);
	}

	return (error_none);
}


/* ----| Publics    |----- */

int	list(
	const Config *const config
)
{
	const char	*s = config->lib.modules.length > 1 ? "s" : "";
	int			err = 0;
	t_array		arr = {0};
	t_display	display = {0};
	/**
	 * TODO:
	 * - parse every modules and file the new `display` struct
	 * - get the max for every module
	 * - dissplay it.
	*/

	if (unlikely(array_alloc(&arr, 32)))
	{
		perror("mem_alloc");
		return (-errno);
	}
	err = _parse_all(config, &arr, &display);
	if (unlikely(err))
		return (err);

	printf("Clib: Module%s available%s: %u\n", s, s, config->lib.modules.length);
	if (config->sub.list.options.stats)
		printf("%-*s %*s %*s %*s\n", display.max_name, "Module", display.max_deps, "Deps", display.max_files, "Files", display.max_size, "Size");
	for (size_t i = 0; i < arr.length; i++)
	{
		struct s_mod	*this = arr.data[i];
		printf("%-*s %*d %*d %*.0f%s\n",
			(int)display.max_name, this->name,
			(int)display.max_deps, this->nb_deps,
			(int)display.max_files, this->nb_files,
			(int)(display.max_size - strlen(this->unit.name)), (double)this->size / this->unit.divisor, this->unit.name
		);
		if (this->nb_deps)
		{
			for (int i = 0; i < this->nb_deps; i++)
			{
				const char *const	prefix = i + 1 == this->nb_deps ? "└─" : "├─";
				printf("%s%s\n", prefix, this->deps[i]);
			}
		}
	}

	array_free(&arr, true);
	return (err);
}
