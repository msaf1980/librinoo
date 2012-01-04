/**
 * @file   buffer_extend.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Thu Jan 21 18:27:58 2010
 *
 * @brief  buffer_extend unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int main()
{
	t_buffer *buf;

	buf = buffer_create(10);
	XTEST(buf != NULL);
	XTEST(buf->buf != NULL);
	XTEST(buf->len == 0);
	XTEST(buf->size == 10);
	XTEST(buffer_extend(buf, 0) == 0);
	XTEST(buf != NULL);
	XTEST(buf->buf != NULL);
	XTEST(buf->len == 0);
	XTEST(buf->size == 10);
	XTEST(buffer_extend(buf, 42) == 0);
	XTEST(buf != NULL);
	XTEST(buf->buf != NULL);
	XTEST(buf->len == 0);
	XTEST(buf->size == RINOO_BUFFER_INCREMENT * 2);
	XTEST(buffer_extend(buf, RINOO_BUFFER_INCREMENT * 2 + 42) == 0);
	XTEST(buf != NULL);
	XTEST(buf->buf != NULL);
	XTEST(buf->len == 0);
	XTEST(buf->size == (RINOO_BUFFER_INCREMENT * 2 + 42) * 2);
	buffer_destroy(buf);
	XPASS();
}
