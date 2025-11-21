// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
	//...

/* -----| Internals |----- */
	//...

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */

typedef struct s_fmt	t_fmt;	// interface/io/printf parsing struct

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

enum e_file_printf_append_access
{
	e_file_printf_append_char,
	e_file_printf_append_str,
};

enum e_printf_flag_localisation
{
	_printf_arg_arg = -2,
};

enum e_printf_flag_format
{
	_printf_flag_format_align,
	_printf_flag_format_sign,
	_printf_flag_format_space,
	_printf_flag_format_alternative,
	_printf_flag_format_padd0,
	_printf_flag_format_width,
	_printf_flag_format_percition,
	_printf_flag_format_percition_arg,
};

enum e_printf_flag_data
{
	_printf_flag_data_char	= 'c',
	_printf_flag_data_str	= 's',
	_printf_flag_data_ptr	= 'p',
	_printf_flag_data_dec	= 'd',
	_printf_flag_data_int	= 'i',
	_printf_flag_data_uint	= 'u',
	_printf_flag_data_oct	= 'o',
	_printf_flag_data_lhex	= 'x',
	_printf_flag_data_hhex	= 'X',
	_printf_flag_data_float	= 'f',
	_printf_flag_data_Float	= 'F',
	_printf_flag_data_exp	= 'e',
	_printf_flag_data_Exp	= 'E',
	_printf_flag_data_gen	= 'g',
	_printf_flag_data_Gen	= 'G',
	_printf_flag_data_hexfl	= 'a',
	_printf_flag_data_Hexfl	= 'A',
	_printf_flag_data_count	= 'n',
	_printf_flag_data_pct	= '%',
};


/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

struct s_fmt
{
	char	flag_minus;
	char	flag_plus;
	char	flag_space;
	char	flag_hash;
	char	flag_zero;

	int		width;
	int		precision;   // -1 si absente

	char	length[3];   // ex: "l", "ll", "h", "hh", ...
	char	specifier;   // ex: 'd', 's', 'f', ...
};

