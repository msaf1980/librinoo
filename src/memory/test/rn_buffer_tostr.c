/**
 * @file   rn_buffer_tostr.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rn_buffer_tostr unit test
 *
 *
 */

#include "rinoo/rinoo.h"

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int main()
{
	rn_buffer_t *buffer;
	char *result;

	buffer = rn_buffer_create(NULL);
	XTEST(buffer != NULL);
	XTEST(rn_buffer_add(buffer, "qwertyuiop", 10) == 10);
	result = rn_buffer_tostr(buffer);
	XTEST(result != NULL);
	XTEST(strlen(result) == 10);
	XTEST(strcmp(result, "qwertyuiop") == 0);
	XTEST(rn_buffer_size(buffer) == 11);
	rn_buffer_destroy(buffer);
	XPASS();
}
