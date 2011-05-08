/**
 * @file   buffer_tostr.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Thu Apr  7 18:20:26 2011
 *
 * @brief  buffer_tostr unit test
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
int		main()
{
  t_buffer	*buf;
  char		*result;

  buf = buffer_create(10, 0);
  XTEST(buf != NULL);
  XTEST(buffer_add(buf, "qwertyuiop", 10) == 10);
  result = buffer_tostr(buf);
  XTEST(result != NULL);
  XTEST(strlen(result) == 10);
  XTEST(strcmp(result, "qwertyuiop") == 0);
  XTEST(buffer_len(buf) == 11);
  buffer_destroy(buf);
  XPASS();
}
