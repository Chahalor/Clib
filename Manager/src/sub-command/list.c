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

static inline int	get_max_size(
	const t_array *const modules
)
{
	int	result = 0;

	for (int i = 0; i < modules->length; i++)
	{
		const int length = strlen((char *)modules->data[i]);

		if (length > result)
			result = length;
	}

	return (result);
}

/* ----| Publics    |----- */

int	list(
	const Config *const config
)
{
	const char	*s = config->lib.modules.length > 1 ? "s" : "";
	int			err = 0;
	t_display	display = {0};
	/**
	 * TODO:
	 * - parse every modules and file the new `display` struct
	 * - get the max for every module
	 * - dissplay it.
	*/


	printf("Clib: Module%s available%s: %u\n", s, s, config->lib.modules.length);
	if (config->sub.list.options.stats)
		printf("     Module     Deps     Files     Size\n");
	for (size_t i = 0; i < config->lib.modules.length; i++)
	{
		const t_module	*const	this = config->lib.modules.data[i];
		const int				nb_deps = this->dependencies.length;

		printf(FORMAT_MODULE, this->name);
		if (config->sub.list.options.version)
		{
			const char *const	version = config->sub.list.options.version
											? toml_get(config->lib.toml, "%s.version", this->name)
											: "";

			printf(FORMAT_VERSION, version);
		}

		if (config->sub.list.options.stats)
		{
			char					path[PATH_MAX];
			snprintf(path, sizeof(path), "%s/%s", config->consts.path_cache_dir, this->path);
			const int			nb_files = count_dir_files(path, true);
			const size_t		size = dir_size(path);
			const t_size_unit	unit = get_unit(size);

			printf(FORMAT_STATS, nb_deps, nb_files, size / unit.divisor, unit.name);
		}

		printf("\n");
		if (config->sub.list.options.deps && this->dependencies.length)
		{
			for (size_t j = 0; j < this->dependencies.length; j++)
			{
				const TOML	*const	dep = this->dependencies.data[j];
				const char *const	prefix = j + 1 == this->dependencies.length ? "   └─" : "   ├─";

				printf("%s %s\n", prefix, dep && dep->data ? dep->data : "(null)");
			}
		}
	}

	return (err);
}
