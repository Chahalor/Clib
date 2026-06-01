/**
 * 
*/
/* ----| Headers    |----- */
	/* Standard */
#include <errno.h>
#include <string.h>
#include <stdio.h>

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

	err = mkdir_p(config->dest, 0777);
	if (unlikely(err < 0))
	{
		array_free(&modules, false);
		return (err);
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
