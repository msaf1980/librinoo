/**
 * @file   fifo_pop.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2010
 * @date   Wed Apr 28 16:42:20 2010
 *
 * @brief  fifo_pop unit test
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
  XTEST(fifo_pop(fifo) == testbuf);
  XTEST(fifo->head == NULL);
  XTEST(fifo->tail == NULL);
  XTEST(fifo->size == 0);
  xfree(testbuf);
  fifo_destroy(fifo);
  XPASS();
}
