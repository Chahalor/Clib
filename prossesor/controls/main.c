/**
 * 
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "../../standards/formating.h"
#include "../prossesor.h"

typedef int	bob;

enum {A, B};

int	g_nb_tests	= 0;
int	g_nb_success = 0;
int	g_nb_fail	= 0;

#define	TEST(name, test, result) {\
	g_nb_tests++;\
	if (test == result)\
	{\
		g_nb_success++;\
		printf("%-32s: [" GREEN "OK" RESET "]\n", name);\
	}\
	else\
	{\
		g_nb_fail++;\
		printf("%-32s: [" RED "KO" RESET "]\n", name);\
	}\
}

int	main(void)
{
	TEST("int is a number",						_is_number(int),				true);
	TEST("uint16_t is a number",				_is_number(uint16_t),			true);
	TEST("void * is not a number",				_is_number(void *),				false);
	TEST("void is not a number",				_is_number(void),				false);
	TEST("typdef bob is a number",				_is_number(bob),				true);
	TEST("int is signed",						IS_SIGNED_TYPE(int),			true);
	TEST("unsigned int is not signed",			IS_SIGNED_TYPE(unsigned int),	false);
	TEST("float is a number",					_is_number(float),				true);
	TEST("double is a number",					_is_number(double),				true);
	TEST("long double is a number",				_is_number(long double),		true);

	TEST("char is a number",					_is_number(char),				true);
	TEST("signed char is a number",				_is_number(signed char),		true);
	TEST("unsigned char is a number",			_is_number(unsigned char),		true);

	TEST("short is a number",					_is_number(short),				true);
	TEST("unsigned short is a number",			_is_number(unsigned short),		true);

	TEST("long is a number",					_is_number(long),				true);
	TEST("unsigned long is a number",			_is_number(unsigned long),		true);

	TEST("long long is a number",				_is_number(long long),			true);
	TEST("unsigned long long is a number",		_is_number(unsigned long long),	true);

	TEST("int32_t is a number",					_is_number(int32_t),			true);
	TEST("uint64_t is a number",				_is_number(uint64_t),			true);

	TEST("pointer to int is not a number",		_is_number(int *),				false);
	TEST("pointer to char is not a number",		_is_number(char *),				false);

	TEST("array is not a number",				_is_number(int[10]),			false);
	TEST("function pointer is not a number",	_is_number(int (*)(void)),		false);

	TEST("enum is a number",					_is_number(A),					true);

	TEST("const int is a number",				_is_number(const int),			true);
	TEST("volatile int is a number",			_is_number(volatile int),		true);

	TEST("char[4] is not a number",				_is_number("bob"),				false);


	printf("nb Tests: %d | nb Success: %d | nb Fails: %d\n", g_nb_tests, g_nb_success, g_nb_fail);
	return 0;
}
