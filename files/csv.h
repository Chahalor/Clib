// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stdio.h>

/* -----| Globals   |----- */
# include "lib_config.h"
# include "lib_standards.h"

/* -----| Internals |----- */
# include "csv/types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

#define	csv_foreach(var, csv)				_CSV_FOREACH_FULL(var, node)
#define	csv_for_column(var, csv, column)	_CSV_FOREACH_COLUMN(var, csv, column)

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                 Open / Close                               */
/* ************************************************************************** */

/* Open from file */
CSV		*csv_open(
			const char *const restrict _filename
			);

/* Open from memory */
CSV		*csv_open_mem(const char *buffer, size_t size);

/* Create empty CSV */
CSV		*csv_create(void);

/* Close & free */
void	csv_close(CSV *csv);

/* ************************************************************************** */
/*                                 State / Info                               */
/* ************************************************************************** */

size_t		csv_row_count(
				const CSV *csv
				);

size_t		csv_column_count(
				const CSV *csv
				);

const char	*csv_column_name(
				const CSV *csv,
				size_t column
				);

int			csv_column_type(
				const CSV *csv,
				size_t column
				);

/* ************************************************************************** */
/*                                 Column Management                          */
/* ************************************************************************** */

int	csv_add_column(
		CSV *csv,
		const char *name,
		int type
		);

int	csv_remove_column(
		CSV *csv,
		const char *name
		);

/* ************************************************************************** */
/*                                 Row Management                             */
/* ************************************************************************** */

/* Append a full row (FILE-like varargs) */
int	csv_append_row(CSV *csv, ...);

/* Explicit typed append (safe alternative) */
int	csv_append_row_ex(
	CSV *csv,
	const void *const *values
);

/* ************************************************************************** */
/*                                 Cell Access                                */
/* ************************************************************************** */

/* Typed getters (safe, no void *) */
int	csv_get_int(
	const CSV *csv,
	size_t row,
	size_t col,
	int *out
);

int	csv_get_string(
	const CSV *csv,
	size_t row,
	size_t col,
	const char **out
);

int	csv_get_time(
	const CSV *csv,
	size_t row,
	size_t col,
	time_t *out
);

/* ************************************************************************** */
/*                                 Iteration (FILE-like)                     */
/* ************************************************************************** */

// int	csv_iter_init(csv_iter_t *it, const CSV *csv);
// int	csv_iter_next(csv_iter_t *it);

// size_t	csv_iter_row(const csv_iter_t *it);
// int	csv_iter_get_int(const csv_iter_t *it, size_t col, int *out);
// int	csv_iter_get_string(const csv_iter_t *it, size_t col, const char **out);
// int	csv_iter_get_time(const csv_iter_t *it, size_t col, time_t *out);

/* ************************************************************************** */
/*                                 Dump / Serialize                           */
/* ************************************************************************** */

int		csv_dump(
			const CSV *csv,
			FILE *stream
			);

char	*csv_stringify(const CSV *csv);

/* ************************************************************************** */
/*                                 Errors                                     */
/* ************************************************************************** */

int			csv_error(const CSV *csv);
const char	*csv_strerror(const CSV *csv);