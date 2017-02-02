/**
 * @file   rn_buffer_init.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rn_buffer_init unit test
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
	rn_buffer_t buffer;
	char memory[25];

	rn_buffer_init(&buffer, memory, sizeof(memory));
	XTEST(buffer.ptr == memory);
	XTEST(buffer.class != NULL);
	XTEST(buffer.size == 0);
	XTEST(buffer.msize == sizeof(memory));
	XTEST(rn_buffer_add(&buffer, "blablabla", 9) == 9);
	XTEST(buffer.ptr != NULL);
	XTEST(memcmp(memory, "blablabla", 9) == 0);
	XTEST(buffer.size == 9);
	XTEST(buffer.msize == sizeof(memory));
	XTEST(rn_buffer_add(&buffer, "blablabla", 9) == 9);
	XTEST(buffer.ptr != NULL);
	XTEST(memcmp(memory, "blablablablablabla", 18) == 0);
	XTEST(buffer.size == 18);
	XTEST(buffer.msize == sizeof(memory));
	XTEST(rn_buffer_add(&buffer, "blablabla", 9) == -1);
	XTEST(buffer.ptr != NULL);
	XTEST(buffer.size == 18);
	XTEST(buffer.msize == sizeof(memory));
	XPASS();
}
