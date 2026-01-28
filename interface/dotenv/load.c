// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "_internal_/_dotenv.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */

/**
 * @brief	load the file .env into the programm env
 */
int	dotenv_load(void)
{
	return (_dotenv_load_file(".env"));
}

/**
 * @brief	load the file at `path` into the env
 */
int	dotenv_load_file(
	const char *const _path
)
{
	return (_dotenv_load_file(_path));
}
