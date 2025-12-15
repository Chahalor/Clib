// Big Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stddef.h>

/* -----| Globals   |----- */
# include "lib_config.h"
# include "lib_standards.h"

/* -----| Internals |----- */
# include "types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

# define FILE_BUFFER_READ_SIZE	1024
# define FILE_BUFFER_WRITE_SIZE	1024

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/* ---| File management |--- */

/**
 * @brief	open the s_file struct from the _filename args
 * 
 * @param	_filename	the file to be opened
 * @param	_mode		mode to open the file (from fcntl.h)
 * @param	...			args needed based on the _mode set
 * 
 * @return	s_file if succesfull open, NULL otherwith.
 */
s_file	*file_open(
			const char *const restrict _filename,
			const int _mode,
			...
			);

/**
 * @brief	close and free the s_file struct
 * 
 * @param	_file	the s_file struct to be closed
 * 
 * @return	0 on success, error code otherwise
 */
int		file_close(
			s_file *const restrict _file
			);

/**
 * @brief	check if the _filename file is already opened and return the mode
 * 
 * @param	_filename	the file to be checked
 * 
 * @return the mode of the file and 0 if not opened
*/
int		file_is_open(
			const char *const restrict _filename
			);

/* ---| File Writing |--- */

/**
 * @brief	write _size char from the _file and put the readed values to _buffer
 * 
 * @param	_file	the file where to read
 * @param	_buffer	the buffer where to put the readed value
 * @param	_size	the number of char to be read
 * 
 * @return	the number of char readed, <0 in case of error
 */
ssize_t	file_write(
			s_file *const restrict _file,
			const void *const restrict _buffer,
			const size_t _size
			);

/**
 * @brief	print the formated string the the _file
 * 
 * @param	_file	the file where to print the string
 * @param	_format	the format string to build the final string
 * @param	...		all argument for fullfiling the string
 * 
 * @return	number of char writed, <0 in case of error
 * 
 * TODO bufferize output based on a '\n' our the buffer filling 
 */
ssize_t	file_printf(
			s_file *const restrict _file,
			const char *const restrict _format,
			...
			);

/* ---| File Reading |--- */

/**
 * @brief	read _size char form _file and store the readed value in _buffer
 * 
 * @param	_file	the file where to read the value
 * @param	_buffer	the buffer to store the readed values
 * @param	_size
 * 
 * @return	number if char readed, <0 in case of error
 */
int		file_read(
			s_file *const restrict _file,
			void *const restrict _buffer,
			const size_t _size
			);

/**
 * @brief	read value based on the _format and put them in there respected pointer passed in param
 * 
 * @param	_file	the file where to read
 * @param	_format	the format to read
 * @param	...		all pointer to store values
 * 
 * @return	number of value readed, <0 in case of error
 */
int		file_readf(
			s_file *const restrict _file,
			const char *const restrict _format,
			...
			);
