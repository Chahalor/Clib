/**
 * @file	times.h
 * @brief	public API for the time module of the lib
 * 
 * this module contaigne function(s) based on time manipulation and conversion
 * 
 * @ingroup time
 */

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
	//...

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

/**
 * @enum	time_unity
 * @brief	all supported unity for the times module
 * 
 * @ingroup times
 */
enum e_time_unity
{
	time_unity_ns,
	time_unity_us,
	time_unity_ms,
	time_unity_sec,
	time_unity_min,
	time_unity_hour,
	time_unity_day,
};

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

/**
 * @brief				convert `src` from one time unit to an other
 * 
 * @param	src			the base value to be converted
 * @param	base_src	the base unit of src
 * @param	dest_src	the unit to be converted in
 * 
 * @return				the converted value
 * 
 * @pre					base_src and base_dest should be from the enum `e_time_unity`
 * 
 * @ingroup times
 * @version	1.0.0
 */
time_t	time_convert(
			const time_t	_src,
			const __u_char	_base_src,
			const __u_char	_base_dest
			);