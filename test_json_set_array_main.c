// Header

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>

	/* Internal */
#include "json.h"

	/* Modules */
#include "memory.h"

	/* External */
		//...

static int	expect_contains(
	const char *const haystack,
	const char *const needle
)
{
	if (!haystack || !needle)
		return (1);
	return (strstr(haystack, needle) == NULL);
}

int	main(void)
{
	JSON	*json = json_new();
	char	*dump = NULL;
	int		err = error_none;

	const char	*names[] = {"bob", "alice", "eve"};
	const int	nums[] = {1, 2, 3};
	const int	flags[] = {1, 0, 1};

	if (!json)
		return (1);

	err = _json_set_array_va_args(json, "users", (void *)names, 3, json_tok_str);
	if (err != error_none)
		return (2);

	err = json_set_from_array(json, "numbers", (void *)nums, 3, json_tok_nbr);
	if (err != error_none)
		return (3);

	err = json_set_from_array(json, "flags", (void *)flags, 3, json_tok_bool);
	if (err != error_none)
		return (4);

	err = _json_set_array_va_args(json, "nulls", NULL, 3, json_tok_null);
	if (err != error_none)
		return (5);

	dump = json_stringify(json, 0);
	if (!dump)
		return (6);

	json_dump(json, stdout, 4);

	if (expect_contains(dump, "\"users\""))
		return (10);
	if (expect_contains(dump, "\"numbers\""))
		return (11);
	if (expect_contains(dump, "\"flags\""))
		return (12);
	if (expect_contains(dump, "\"nulls\""))
		return (13);

	if (strcmp((char *)json_get(json, "users.0"), "bob") != 0)
		return (20);
	if (json_get(json, "numbers.0") == NULL)
		return (21);
	if (json_get(json, "flags.1") == NULL)
		return (22);
	if (json_get(json, "nulls.2") != NULL)
		return (23);

	mem_free(dump);
	json_unload(json);
	return (0);
}
