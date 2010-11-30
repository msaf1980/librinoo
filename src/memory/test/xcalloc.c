/**
 * @file   xcalloc.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2010
 * @date   Thu Jan 21 18:18:54 2010
 *
 * @brief  xcalloc unit test
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

  ptr = xcalloc(42, 42);
  XTEST(ptr != NULL);
  xfree(ptr);
  ptr = xcalloc(0, 42);
  XTEST(ptr == NULL);
  ptr = xcalloc(42, -42);
  XTEST(ptr == NULL);
  XPASS();
}
