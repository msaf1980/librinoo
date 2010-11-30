/**
 * @file   xrealloc.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2010
 * @date   Thu Jan 21 18:18:54 2010
 *
 * @brief  xrealloc unit test
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
  void		*ptr;

  ptr = xmalloc(42);
  XTEST(ptr != NULL);
  ptr = xrealloc(ptr, 84);
  XTEST(ptr != NULL);
  xfree(ptr);
  ptr = xrealloc(ptr, -42);
  XTEST(ptr == NULL);
  ptr = xrealloc(NULL, 42);
  XTEST(ptr == NULL);
  XPASS();
}
