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

	/* External */
#include "files/toml.h"
#include "interface/args.h"

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

static int	_target_from_path(
	const char *const	path,
	char *const			target,
	const size_t		size
)
{
	size_t	i;
	size_t	j;

	if (unlikely(!path || !*path || !target || size < sizeof("clib_")))
		return (-EINVAL);

	else if (!strcmp(path, "_internal_"))
		return (snprintf(target, size, "clib_internal") >= (int)size ? -ENAMETOOLONG : error_none);

	else if (!strcmp(path, "network/communication"))
		return (snprintf(target, size, "clib_comm") >= (int)size ? -ENAMETOOLONG : error_none);

	else if (!strcmp(path, "network/protocols"))
		return (snprintf(target, size, "clib_files_protocols") >= (int)size ? -ENAMETOOLONG : error_none);

	else if (snprintf(target, size, "clib_") >= (int)size)
		return (-ENAMETOOLONG);

	i = 0;
	j = strlen(target);
	while (path[i])
	{
		if (unlikely(j + 1 >= size))
			return (-ENAMETOOLONG);
		target[j++] = (path[i] == '/' || path[i] == '-') ? '_' : path[i];
		++i;
	}
	target[j] = '\0';

	return (error_none);
}

static int	_target_from_cmake(
	const Config *const	config,
	const char *const	module_path,
	char *const			target,
	const size_t		size
)
{
	char	path[PATH_MAX];
	char	line[1024];
	char	*start;
	char	*end;
	FILE	*file;
	size_t	len;

	snprintf(path, sizeof(path), "%s/%s/CMakeLists.txt", config->dest ? config->dest : ".", module_path);
	file = fopen(path, "r");
	if (unlikely(!file))
		return (_target_from_path(module_path, target, size));

	while (fgets(line, sizeof(line), file))
	{
		start = strstr(line, "add_library");
		if (!start)
			continue ;

		start = strchr(start, '(');
		if (!start)
			continue ;

		++start;
		while (*start == ' ' || *start == '\t')
			++start;
		end = start;
		while (*end && *end != ')' && *end != ' ' && *end != '\t'
			&& *end != '\n')
			++end;

		len = end - start;
		if (!len || len >= size)
			break ;

		memcpy(target, start, len);
		target[len] = '\0';

		fclose(file);
		return (error_none);
	}

	fclose(file);
	return (_target_from_path(module_path, target, size));
}

static inline void	_write_header(
	FILE *const file
)
{
	fprintf(file,
		"cmake_minimum_required(VERSION 3.15)\n"
		"\n"
		"project(clib C)\n"
		"\n"
		"set(CMAKE_C_STANDARD 11)\n"
		"set(CMAKE_POSITION_INDEPENDENT_CODE ON)\n"
		"if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)\n"
		"\tset(CMAKE_BUILD_TYPE Debug CACHE STRING \"Build type\" FORCE)\n"
		"endif()\n"
		"\n"
		"add_library(clib STATIC)\n"
		"\n"
		"add_library(clib_includes INTERFACE)\n"
		"\n"
	);
}

static void	_write_include_path(
	FILE *const			file,
	const char *const	path
)
{
	char	buffer[PATH_MAX];
	char	*slash;

	if (unlikely(!path || !*path || snprintf(buffer, sizeof(buffer), "%s", path) >= (int)sizeof(buffer)))
		return ;

	while (*buffer)
	{
		fprintf(file, "\t${CMAKE_CURRENT_SOURCE_DIR}/%s\n", buffer);
		slash = strrchr(buffer, '/');
		if (!slash)
			break ;
		*slash = '\0';
	}
}

static inline void	_write_include_directories(
	const Config *const	config,
	FILE *const			file
)
{
	fprintf(file,
		"target_include_directories(clib PUBLIC\n"
		"\t${CMAKE_CURRENT_SOURCE_DIR}\n"
		")\n"
		"\n"
		"target_include_directories(clib_includes INTERFACE\n"
		"\t${CMAKE_CURRENT_SOURCE_DIR}\n"
		"\t${CMAKE_CURRENT_SOURCE_DIR}/_internal_\n"
		"\t${CMAKE_CURRENT_SOURCE_DIR}/standards\n"
	);

	for (size_t i = 0; i < config->conf.modules.length; i++)
	{
		t_module	*const this = (t_module *const)config->conf.modules.data[i];

		_write_include_path(file, this->path);
	}
	fprintf(file, ")\n\n");
}

static int	_write_module(
	const Config *const	config,
	FILE *const			file,
	const t_module *const	module
)
{
	char	target[PATH_MAX];
	int		err;

	if (unlikely(!module || !module->path || !*module->path))
		return (-EINVAL);

	err = _target_from_cmake(config, module->path, target, sizeof(target));
	if (unlikely(err))
		return (err);

	fprintf(file,
		"if (EXISTS \"${CMAKE_CURRENT_SOURCE_DIR}/%s/CMakeLists.txt\")\n"
		"\tadd_subdirectory(\"%s\")\n"
		"else()\n"
		"\tmessage(FATAL_ERROR \"Required Clib module '%s' is missing\")\n"
		"endif()\n"
		"\n"
		"if (TARGET %s)\n"
		"\tget_target_property(_clib_target_type %s TYPE)\n"
		"\tif (_clib_target_type STREQUAL \"OBJECT_LIBRARY\")\n"
		"\t\ttarget_sources(clib PRIVATE $<TARGET_OBJECTS:%s>)\n"
		"\telse()\n"
		"\t\ttarget_link_libraries(clib PRIVATE %s)\n"
		"\tendif()\n"
		"else()\n"
		"\tmessage(FATAL_ERROR \"Required Clib module '%s' did not create target '%s'\")\n"
		"endif()\n"
		"\n",
		module->path, module->path, module->path,
		target, target, target, target, module->path, target);

	return (error_none);
}

static int	_write_sub_directories(
	const Config *const	config,
	FILE *const			file
)
{
	int	err;

	fprintf(file,
		"if (EXISTS \"${CMAKE_CURRENT_SOURCE_DIR}/_internal_/CMakeLists.txt\")\n"
		"\tadd_subdirectory(_internal_)\n"
		"\ttarget_sources(clib PRIVATE $<TARGET_OBJECTS:clib_internal>)\n"
		"endif()\n"
		"\n"
		"if (EXISTS \"${CMAKE_CURRENT_SOURCE_DIR}/standards/CMakeLists.txt\")\n"
		"\tadd_subdirectory(standards)\n"
		"\ttarget_link_libraries(clib PRIVATE clib_standards)\n"
		"endif()\n"
		"\n"
	);

	for (size_t i = 0; i < config->conf.modules.length; i++)
	{
		t_module	*const this = (t_module *const)config->conf.modules.data[i];

		err = _write_module(config, file, this);
		if (unlikely(err))
			return (err);
	}
	return (error_none);
}

static inline void	_write_footer(
	FILE *const	file
)
{
	fprintf(file,
		"target_compile_options(clib\n"
		"\tPUBLIC\n"
		"\t\t-Wall\n"
		"\t\t-Wextra\n"
		"\t\t-Werror\n"
		"\t\t-g3\n"
		"\t\t$<$<CONFIG:Debug>:-O0>\n"
		"\t\t$<$<CONFIG:Debug>:-g3>\n"
		"\t\t$<$<CONFIG:Release>:-O2>\n"
		"\t\t$<$<CONFIG:RelWithDebInfo>:-O2>\n"
		")\n"
		"\n"
		"target_compile_definitions(clib\n"
		"\tPUBLIC\n"
		"\t\t$<$<CONFIG:Debug>:CLIB_DEBUG>\n"
		")\n"
	);
}

/* ----| Public     |----- */

int	cmake_write(
	const Config *const	config
)
{
	char	path[PATH_MAX] = {0};
	FILE	*file = NULL;
	int		err;

	if (unlikely(!config))
		return (-EINVAL);

	if (unlikely(snprintf(path, sizeof(path), "%s/CMakeLists.txt", config->dest ? config->dest : ".") >= (int)sizeof(path)))
		return (-ENAMETOOLONG);

	file = fopen(path, "w");
	if (unlikely(!file))
	{
		fprintf(stderr, "could not open %s: %s\n", path, strerror(errno));
		return (-errno);
	}

	_write_header(file);
	_write_include_directories(config, file);
	err = _write_sub_directories(config, file);
	if (unlikely(err))
	{
		fclose(file);
		return (err);
	}
	_write_footer(file);

	fclose(file);
	return (error_none);
}
