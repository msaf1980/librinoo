/**
 * @file   rn_buffer_vprint.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rn_buffer_vprint unit test
 *
 *
 */

#include "rinoo/rinoo.h"

static rn_buffer_class_t test_class = {
	.inisize = 10,
	.maxsize = RN_BUFFER_HELPER_MAXSIZE,
	.init = NULL,
	.growthsize = rn_buffer_helper_growthsize,
	.malloc = rn_buffer_helper_malloc,
	.realloc = rn_buffer_helper_realloc,
	.free = rn_buffer_helper_free,
};

int test_vprint(rn_buffer_t *buffer, const char *format, ...)
{
	int res;
	va_list ap;

	va_start(ap, format);
	res = rn_buffer_vprint(buffer, format, ap);
	va_end(ap);
	return res;
}

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int main()
{
	rn_buffer_t *buffer;

	buffer = rn_buffer_create(&test_class);
	XTEST(buffer != NULL);
	XTEST(buffer->ptr != NULL);
	XTEST(buffer->size == 0);
	XTEST(buffer->msize == 10);
	XTEST(test_vprint(buffer, "42 %s", "42") == 5);
	XTEST(buffer->ptr != NULL);
	XTEST(memcmp(buffer->ptr, "42 42", 5) == 0);
	XTEST(buffer->size == 5);
	XTEST(buffer->msize == 10);
	XTEST(test_vprint(buffer, " 42 %s %d", "42", 42) == 9);
	XTEST(buffer->ptr != NULL);
	XTEST(memcmp(buffer->ptr, "42 42 42 42 42", 14) == 0);
	XTEST(buffer->size == 14);
	XTEST(buffer->msize == buffer->class->growthsize(buffer, 14));
	rn_buffer_destroy(buffer);
	test_class.inisize = 4;
	buffer = rn_buffer_create(&test_class);
	XTEST(buffer != NULL);
	XTEST(buffer->ptr != NULL);
	XTEST(buffer->size == 0);
	XTEST(buffer->msize == 4);
	XTEST(test_vprint(buffer, "4242") == 4);
	XTEST(buffer->ptr != NULL);
	XTEST(memcmp(buffer->ptr, "4242", 4) == 0);
	XTEST(buffer->size == 4);
	XTEST(buffer->msize == buffer->class->growthsize(buffer, 4));
	rn_buffer_destroy(buffer);
	XPASS();
}
