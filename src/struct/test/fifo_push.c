/**
 * @file   fifo_push.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Apr 28 16:42:20 2010
 *
 * @brief  fifo_push unit test
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
  t_fifo	*fifo2;

  fifo = fifo_create();
  XTEST(fifo != NULL);
  XTEST(fifo_push(fifo, (void *) 42, NULL) == TRUE);
  XTEST(fifo->head != NULL);
  XTEST(fifo->tail != NULL);
  XTEST(fifo->head == fifo->tail);
  XTEST(fifo->size == 1);
  XTEST(fifo->head->node == (void *) 42);
  fifo2 = fifo_create();
  XTEST(fifo_push(fifo2, fifo, fifo_destroy) == TRUE);
  fifo_destroy(fifo2);
  XPASS();
}
