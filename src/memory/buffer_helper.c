/**
 * @file   buffer_helper.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Buffer helper functions
 *
 *
 */

#include "rinoo/memory/module.h"

size_t rn_buffer_helper_growthsize(rn_buffer_t *buffer, size_t newsize)
{
	if (newsize >= buffer->class->maxsize) {
		return buffer->class->maxsize;
	}
	if (newsize < buffer->msize) {
		return buffer->msize;
	}
	return (size_t)(newsize * 1.5);
}

void *rn_buffer_helper_malloc(rn_buffer_t *unused(buffer), size_t size)
{
	return malloc(size);
}

void *rn_buffer_helper_realloc(rn_buffer_t *buffer, size_t newsize)
{
	return realloc(buffer->ptr, newsize);
}

int rn_buffer_helper_free(rn_buffer_t *buffer)
{
	free(buffer->ptr);
	buffer->ptr = NULL;
	return 0;
}
