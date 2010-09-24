/**
 * @file   list_add.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
 * @date   Wed Apr 28 16:42:20 2010
 *
 * @brief  list_add unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

int		cmp_func(void *unused(node1), void *unused(node2))
{
  return (1);
}

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int		main()
{
  t_list	*list;
  t_list	*list2;

  list = list_create(cmp_func);
  XTEST(list != NULL);
  XTEST(list_add(list, (void *) 42, NULL) != NULL);
  XTEST(list->head != NULL);
  XTEST(list->tail != NULL);
  XTEST(list->head == list->tail);
  XTEST(list->size == 1);
  XTEST(list->head->node == (void *) 42);
  XTEST(list_add(list, (void *) 43, NULL) != NULL);
  XTEST(list->head != NULL);
  XTEST(list->tail != NULL);
  XTEST(list->head != list->tail);
  XTEST(list->size == 2);
  XTEST(list->head->node == (void *) 42);
  XTEST(list->tail->node == (void *) 43);
  XTEST(list_add(list, (void *) 44, NULL) != NULL);
  XTEST(list->head != NULL);
  XTEST(list->tail != NULL);
  XTEST(list->head != list->tail);
  XTEST(list->size == 3);
  XTEST(list->head->node == (void *) 42);
  XTEST(list->tail->node == (void *) 44);
  XTEST(list_add(list, (void *) 45, NULL) != NULL);
  XTEST(list->head != NULL);
  XTEST(list->tail != NULL);
  XTEST(list->head != list->tail);
  XTEST(list->size == 4);
  XTEST(list->head->node == (void *) 42);
  XTEST(list->tail->node == (void *) 45);
  list2 = list_create(cmp_func);
  XTEST(list_add(list2, list, list_destroy) != NULL);
  list_destroy(list2);
  XPASS();
}
