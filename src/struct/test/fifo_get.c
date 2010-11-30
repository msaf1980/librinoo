/**
 * @file   fifo_get.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2010
 * @date   Wed Apr 28 16:42:20 2010
 *
 * @brief  fifo_get unit test
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
  t_fifo	*fifo;
  void		*testbuf;

  fifo = fifo_create();
  testbuf = xmalloc(42);
  XTEST(fifo != NULL);
  XTEST(testbuf != NULL);
  XTEST(fifo_push(fifo, testbuf, xfree) == TRUE);
  XTEST(fifo_get(fifo) == testbuf);
  XTEST(fifo->head != NULL);
  XTEST(fifo->tail != NULL);
  XTEST(fifo->size == 1);
  fifo_destroy(fifo);
  XPASS();
}
