/**
 * @file   list_getnext.c
 * @author Reginald LIPS <reginald.l@gmail.com>
 * @date   Thu Nov 25 16:50:14 2010
 *
 * @brief  list_getnext unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

int		cmp_func(void *node1, void *node2)
{
  (void) node1;
  (void) node2;
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
  t_list		*list;
  t_listiterator	iterator = NULL;

  list = list_create(cmp_func);
  XTEST(list != NULL);
  XTEST(list_add(list, (void *) 42, NULL) != NULL);
  XTEST(list_add(list, (void *) 43, NULL) != NULL);
  XTEST(list_add(list, (void *) 44, NULL) != NULL);
  XTEST(list_add(list, (void *) 45, NULL) != NULL);
  XTEST(list->head != NULL);
  XTEST(list->tail != NULL);
  XTEST(list->size == 4);
  XTEST(list->head->node == (void *) 42);
  XTEST(list->tail->node == (void *) 45);
  XTEST(list_getnext(list, &iterator) == (void *) 42);
  XTEST(iterator != NULL);
  XTEST(iterator->node == (void *) 42);
  XTEST(list->head != NULL);
  XTEST(list->tail != NULL);
  XTEST(list->size == 4);
  XTEST(list_getnext(list, &iterator) == (void *) 43);
  XTEST(iterator != NULL);
  XTEST(iterator->node == (void *) 43);
  XTEST(list->head != NULL);
  XTEST(list->tail != NULL);
  XTEST(list->size == 4);
  XTEST(list_getnext(list, &iterator) == (void *) 44);
  XTEST(iterator != NULL);
  XTEST(iterator->node == (void *) 44);
  XTEST(list->head != NULL);
  XTEST(list->tail != NULL);
  XTEST(list->size == 4);
  XTEST(list_getnext(list, &iterator) == (void *) 45);
  XTEST(iterator != NULL);
  XTEST(iterator->node == (void *) 45);
  XTEST(list->head != NULL);
  XTEST(list->tail != NULL);
  XTEST(list->size == 4);
  XTEST(list_getnext(list, &iterator) == NULL);
  XTEST(iterator == NULL);
  XTEST(list->head != NULL);
  XTEST(list->tail != NULL);
  XTEST(list->size == 4);
  list_destroy(list);
  XPASS();
}

