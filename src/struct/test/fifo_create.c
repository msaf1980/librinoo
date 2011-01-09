/**
 * @file   fifo_create.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Apr 28 16:42:20 2010
 *
 * @brief  fifo_create unit test
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

  fifo = fifo_create();
  XTEST(fifo != NULL);
  XTEST(fifo->head == NULL);
  XTEST(fifo->tail == NULL);
  XTEST(fifo->size == 0);
  fifo_destroy(fifo);
  XPASS();
}
