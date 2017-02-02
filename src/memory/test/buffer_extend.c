/**
 * @file   rn_buffer_extend.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rn_buffer_extend unit test
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

	buffer = rn_buffer_create(NULL);
	XTEST(buffer != NULL);
	XTEST(buffer->ptr != NULL);
	XTEST(buffer->size == 0);
	XTEST(buffer->msize == RN_BUFFER_HELPER_INISIZE);
	XTEST(buffer->class != NULL);
	XTEST(rn_buffer_extend(buffer, 0) == 0);
	XTEST(buffer != NULL);
	XTEST(buffer->ptr != NULL);
	XTEST(buffer->size == 0);
	XTEST(buffer->msize == RN_BUFFER_HELPER_INISIZE);
	XTEST(rn_buffer_extend(buffer, RN_BUFFER_HELPER_INISIZE + 42) == 0);
	XTEST(buffer != NULL);
	XTEST(buffer->ptr != NULL);
	XTEST(buffer->size == 0);
	XTEST(buffer->msize == buffer->class->growthsize(buffer, RN_BUFFER_HELPER_INISIZE + 42));
	XTEST(rn_buffer_extend(buffer, RN_BUFFER_HELPER_INISIZE * 2 + 42) == 0);
	XTEST(buffer != NULL);
	XTEST(buffer->ptr != NULL);
	XTEST(buffer->size == 0);
	XTEST(buffer->msize == buffer->class->growthsize(buffer, RN_BUFFER_HELPER_INISIZE * 2 + 42));
	rn_buffer_destroy(buffer);
	XPASS();
}
