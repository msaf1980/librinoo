/**
 * @file   jobqueue_create.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
 * @date   Wed Apr 28 16:42:20 2010
 *
 * @brief  jobqueue_create unit test
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
  t_list	*jobqueue;

  jobqueue = jobqueue_create();
  XTEST(jobqueue != NULL);
  XTEST(jobqueue->head == NULL);
  XTEST(jobqueue->tail == NULL);
  XTEST(jobqueue->size == 0);
  jobqueue_destroy(jobqueue);
  XPASS();
}
