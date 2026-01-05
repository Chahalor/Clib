// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <time.h>

/* -----| Globals   |----- */
	//...

/* -----| Internals |----- */
#include "../../csv.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */
/* -----| typename |----- */
	//...

/* -----| enums    |----- */
	//...

/* -----| unions   |----- */
typedef union u_lib_csv_values_	t_lib_csv_values_;

/* -----| structs  |----- */
typedef struct s_lib_csv_column_	t_csv_columns_;
typedef struct s_lib_csv			CSV;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */

/**
 * @union	lib_csv_values
 * 
 * @brief	unions in charge of storing the values of one element of a column
 */
union u_lib_csv_values_
{
	char	**str;	// tableau de char*
	int		*nbr;	// tableau de int
	time_t	*time;	// tableau de time_t
};


/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

/**
 * @struct	lib_csv_column
 * 
 * @brief	struct in change of storing one columns data
 */
struct s_lib_csv_column_
{
	char						*name;		// name of the column
	t_lib_csv_values_			values;		// values of the columns
	enum e_lib_csv_data_types	type;		// type of the columns
	size_t						length;		// number of element write in the columns
	size_t						capacity;	// total number of elements allocated
};

/**
 * @struct	lib_csv
 * 
 * @brief	interface struct for manipulating csv data
*/
struct s_lib_csv
{
	size_t			nb_column;	// numbers of  columns in the csv
	size_t			nb_row;		// numbers of rows in ths csv
	t_csv_columns_	*columns;	// all columns from this csv
	t_csv_errors	error;		// last error of this csv
};
