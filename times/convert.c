// Header

/* ----| Headers    |----- */
	/* Standard */
		//...

	/* Internal */
#include "times.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

/**
 * @brief	static conversion table from any handeld unit to ns
 */
static const time_t	_g_time_to_ns[] = {
	[time_unity_ns]		= 1LL,
	[time_unity_us]		= 1000LL,
	[time_unity_ms]		= 1000000LL,
	[time_unity_sec]	= 1000000000LL,
	[time_unity_min]	= 60LL * 1000000000LL,
	[time_unity_hour]	= 3600LL * 1000000000LL,
	[time_unity_day]	= 86400LL * 1000000000LL,
};

/* ----| Public     |----- */

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
 * 
 */
time_t	time_convert(
	const time_t	_src,
	const __u_char	_base_src,
	const __u_char	_base_dest
)	
{
	time_t	result = 0;

	if (unlikely(_base_src >= time_unity_day + 1
		|| _base_dest >= time_unity_day + 1))
		return (0);

	result = _src * _g_time_to_ns[_base_src];
	return (result / _g_time_to_ns[_base_dest]);
}
