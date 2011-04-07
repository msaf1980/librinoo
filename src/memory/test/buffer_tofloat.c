/**
 * @file   buffer_tofloat.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Thu Apr  7 18:20:26 2011
 *
 * @brief  buffer_tofloat unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

void		check_buf(t_buffer *buf, float expected_result)
{
  size_t	len;
  float		result;

  result = buffer_tofloat(buf, NULL);
  XTEST(result == expected_result);
  len = 0;
  result = buffer_tofloat(buf, &len);
  XTEST(result == expected_result);
  XTEST(len == buffer_len(buf));
  result = buffer_tofloat(buf, NULL);
  XTEST(result == expected_result);
  len = 0;
  result = buffer_tofloat(buf, &len);
  XTEST(result == expected_result);
  XTEST(len == buffer_len(buf));
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
  t_buffer	buf2;

  buf = buffer_create(10, 0);
  XTEST(buf != NULL);
  XTEST(buffer_add(buf, "12345.6789", 10) == 10);
  check_buf(buf, 12345.6789);
  buffer_erase(buf, buffer_len(buf));
  XTEST(buffer_add(buf, "1", 1) == 1);
  check_buf(buf, 1);
  buffer_erase(buf, buffer_len(buf));
  XTEST(buffer_add(buf, "-12345.678", 10) == 10);
  check_buf(buf, -12345.678);
  buffer_erase(buf, buffer_len(buf));
  buffer_destroy(buf);
  strtobuffer(buf2, "98765.4321");
  check_buf(&buf2, 98765.4321);
  strtobuffer(buf2, "0");
  check_buf(&buf2, 0);
  strtobuffer(buf2, "-9876543.21");
  check_buf(&buf2, -9876543.21);
  XPASS();
}
