/**
 * @file   buffer_vprint.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Thu Jan 21 18:27:58 2010
 *
 * @brief  buffer_vprint unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

int		test_vprint(t_buffer *buf, const char *format, ...)
{
  int		res;
  va_list	ap;

  va_start(ap, format);
  res = buffer_vprint(buf, format, ap);
  va_end(ap);
  return (res);
}

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int		main()
{
  t_buffer	*buf;

  buf = buffer_create(10, 42);
  XTEST(buf != NULL);
  XTEST(buf->buf != NULL);
  XTEST(buf->len == 0);
  XTEST(buf->size == 10);
  XTEST(buf->max_size == 42);
  XTEST(test_vprint(buf, "42 %s", "42") == 5);
  XTEST(buf->buf != NULL);
  XTEST(memcmp(buf->buf, "42 42", 5) == 0);
  XTEST(buf->len == 5);
  XTEST(buf->size == 10);
  XTEST(buf->max_size == 42);
  XTEST(test_vprint(buf, " 42 %s %d", "42", 42) == 9);
  XTEST(buf->buf != NULL);
  XTEST(memcmp(buf->buf, "42 42 42 42 42", 14) == 0);
  XTEST(buf->len == 14);
  XTEST(buf->size == 42);
  XTEST(buf->max_size == 42);
  buffer_destroy(buf);
  XPASS();
}
