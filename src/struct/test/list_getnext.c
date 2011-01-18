/**
 * @file   list_getnext.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
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
  XTEST(list_add(list, INT_TO_PTR(42), NULL) != NULL);
  XTEST(list_add(list, INT_TO_PTR(43), NULL) != NULL);
  XTEST(list_add(list, INT_TO_PTR(44), NULL) != NULL);
  XTEST(list_add(list, INT_TO_PTR(45), NULL) != NULL);
  XTEST(list->head != NULL);
  XTEST(list->tail != NULL);
  XTEST(list->size == 4);
  XTEST(list->head->node == INT_TO_PTR(42));
  XTEST(list->tail->node == INT_TO_PTR(45));
  XTEST(list_getnext(list, &iterator) == INT_TO_PTR(42));
  XTEST(iterator != NULL);
  XTEST(iterator->node == INT_TO_PTR(42));
  XTEST(list->head != NULL);
  XTEST(list->tail != NULL);
  XTEST(list->size == 4);
  XTEST(list_getnext(list, &iterator) == INT_TO_PTR(43));
  XTEST(iterator != NULL);
  XTEST(iterator->node == INT_TO_PTR(43));
  XTEST(list->head != NULL);
  XTEST(list->tail != NULL);
  XTEST(list->size == 4);
  XTEST(list_getnext(list, &iterator) == INT_TO_PTR(44));
  XTEST(iterator != NULL);
  XTEST(iterator->node == INT_TO_PTR(44));
  XTEST(list->head != NULL);
  XTEST(list->tail != NULL);
  XTEST(list->size == 4);
  XTEST(list_getnext(list, &iterator) == INT_TO_PTR(45));
  XTEST(iterator != NULL);
  XTEST(iterator->node == INT_TO_PTR(45));
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

