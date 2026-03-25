/**
 * 
*/

#ifndef LIB_PROSSESOR_H
# define LIB_PROSSESOR_H

# pragma once

# include <stdint.h>

/**
 * @brief	force type promotion
 * 
 * (remove const, volatile, ...)
*/
// # define	_decay(x)	typeof(+(x))
#define _decay(x) typeof((x) + 0)

/**
 * @brief	say if `x` is compatible with `T` using `__builtin_types_compatible_p`
*/
# define	IS_TYPE(x, T)		__builtin_types_compatible_p(typeof(x), T)

/**
 * @brief	say if the type `T` is signed or not
*/
# define	IS_SIGNED_TYPE(T)	(((T)-1) < (T)0)

# if __has_builtin(__builtin_types_compatible_p)

/**
 * @brief	return true if the type `x` is a valid interger
*/
#  define _is_integer(x) (				\
	IS_TYPE(x, char) ||					\
	IS_TYPE(x, signed char) ||			\
	IS_TYPE(x, unsigned char) ||		\
	IS_TYPE(x, int) ||					\
	IS_TYPE(x, unsigned int) ||			\
	IS_TYPE(x, long) ||					\
	IS_TYPE(x, unsigned long) ||		\
	IS_TYPE(x, long long) ||			\
	IS_TYPE(x, unsigned long long) ||	\
	IS_TYPE(x, int8_t) ||				\
	IS_TYPE(x, uint8_t) ||				\
	IS_TYPE(x, int16_t) ||				\
	IS_TYPE(x, uint16_t) ||				\
	IS_TYPE(x, int32_t) ||				\
	IS_TYPE(x, uint32_t) ||				\
	IS_TYPE(x, int64_t) ||				\
	IS_TYPE(x, uint64_t)				\
)

/**
 * @brief	return true if the type `x` if a floating point type
*/
#  define	_is_floating(x) (	\
	IS_TYPE(x, float) ||	\
	IS_TYPE(x, double) ||	\
	IS_TYPE(x, long double)	\
)

#  define	_is_number(x) (_is_integer(x) || _is_floating(x))

/**
 * @brief	return true if `x` is a string
*/
#  define	_is_string(x) (      \
	IS_TYPE(x, char *) ||    \
	IS_TYPE(x, const char *) \
)


# else

/* in the case the compiler dont have __builtin_types_compatible_p has a builtin */
#  define	_is_integer(x)	_Generic((x), \
	char: 1, signed char: 1, unsigned char: 1, \
	int: 1, unsigned int: 1, \
	long: 1, unsigned long: 1, \
	long long: 1, unsigned long long: 1, \
	int8_t: 1, uint8_t: 1, \
	int16_t: 1, uint16_t: 1, \
	int32_t: 1, uint32_t: 1, \
	int64_t: 1, uint64_t: 1, \
	default: 0 \

/**
 * @brief	return true if the type `x` if a floating point type
*/
#  define	_is_floating(x)	_Generic((x), \
	float: 1, const float: 1, \
	double: 1, const double: 1, \
	long double: 1, const long double: 1, \
	default: 0
)

#  define	_is_number(x) (_is_integer(x) || _is_floating(x))

/**
 * @brief	return true if `x` is a string
*/
#  define	_is_string(x)	_Generic((x), \
	char *: 1, const char *: 1, \
	default: 0
)

# endif
/**
 * @brief	return true is `x` is a valid signed number
*/
// #define	_is_signed(x)	(((typeof(x))(-1)) < (typeof(x))0)
#define _is_signed(x) \
__builtin_choose_expr( \
    _is_integer(x), \
    (((_decay(x))(-1)) < (_decay(x))0), \
    0 \
)

#endif	// LIB_PROSSESOR_H
