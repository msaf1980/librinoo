/**
 * @file   list_addnode.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Apr 28 16:42:20 2010
 *
 * @brief  list_addnode unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

int		cmp_func(void *unused(node1), void *unused(node2))
{
  return node1 - node2;
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
  t_listnode	*node;

  /* LIST_SORTED_HEAD test */
  list = list_create(LIST_SORTED_HEAD, cmp_func);
  XTEST(list != NULL);
  node = list_add(list, INT_TO_PTR(42), NULL);
  XTEST(node != NULL);
  XTEST(list_popnode(list, node) == 0);
  XTEST(list_add(list, INT_TO_PTR(44), NULL) != NULL);
  XTEST(list_addnode(list, node) == 0);
  XTEST(node->prev == NULL);
  XTEST(node->next != NULL);
  XTEST(node->next->node == INT_TO_PTR(44));
  XTEST(list_popnode(list, node) == 0);
  XTEST(list_add(list, INT_TO_PTR(43), NULL) != NULL);
  XTEST(list_addnode(list, node) == 0);
  XTEST(node->prev == NULL);
  XTEST(node->next != NULL);
  XTEST(node->next->node == INT_TO_PTR(43));
  XTEST(list_popnode(list, node) == 0);
  XTEST(list_add(list, INT_TO_PTR(40), NULL) != NULL);
  XTEST(list_addnode(list, node) == 0);
  XTEST(node->prev != NULL);
  XTEST(node->prev->node == INT_TO_PTR(40));
  XTEST(node->next != NULL);
  XTEST(node->next->node == INT_TO_PTR(43));
  XTEST(list_popnode(list, node) == 0);
  XTEST(list_add(list, INT_TO_PTR(46), NULL) != NULL);
  XTEST(list_addnode(list, node) == 0);
  XTEST(node->prev != NULL);
  XTEST(node->prev->node == INT_TO_PTR(40));
  XTEST(node->next != NULL);
  XTEST(node->next->node == INT_TO_PTR(43));
  XTEST(list_popnode(list, node) == 0);
  XTEST(list_add(list, INT_TO_PTR(45), NULL) != NULL);
  XTEST(list_addnode(list, node) == 0);
  XTEST(node->prev != NULL);
  XTEST(node->prev->node == INT_TO_PTR(40));
  XTEST(node->next != NULL);
  XTEST(node->next->node == INT_TO_PTR(43));
  list_destroy(list);

  /* LIST_SORTED_TAIL test */
  list = list_create(LIST_SORTED_TAIL, cmp_func);
  XTEST(list != NULL);
  node = list_add(list, INT_TO_PTR(42), NULL);
  XTEST(node != NULL);
  XTEST(list_popnode(list, node) == 0);
  XTEST(list_add(list, INT_TO_PTR(44), NULL) != NULL);
  XTEST(list_addnode(list, node) == 0);
  XTEST(node->prev == NULL);
  XTEST(node->next != NULL);
  XTEST(node->next->node == INT_TO_PTR(44));
  XTEST(list_popnode(list, node) == 0);
  XTEST(list_add(list, INT_TO_PTR(43), NULL) != NULL);
  XTEST(list_addnode(list, node) == 0);
  XTEST(node->prev == NULL);
  XTEST(node->next != NULL);
  XTEST(node->next->node == INT_TO_PTR(43));
  XTEST(list_popnode(list, node) == 0);
  XTEST(list_add(list, INT_TO_PTR(40), NULL) != NULL);
  XTEST(list_addnode(list, node) == 0);
  XTEST(node->prev != NULL);
  XTEST(node->prev->node == INT_TO_PTR(40));
  XTEST(node->next != NULL);
  XTEST(node->next->node == INT_TO_PTR(43));
  XTEST(list_popnode(list, node) == 0);
  XTEST(list_add(list, INT_TO_PTR(46), NULL) != NULL);
  XTEST(list_addnode(list, node) == 0);
  XTEST(node->prev != NULL);
  XTEST(node->prev->node == INT_TO_PTR(40));
  XTEST(node->next != NULL);
  XTEST(node->next->node == INT_TO_PTR(43));
  XTEST(list_popnode(list, node) == 0);
  XTEST(list_add(list, INT_TO_PTR(45), NULL) != NULL);
  XTEST(list_addnode(list, node) == 0);
  XTEST(node->prev != NULL);
  XTEST(node->prev->node == INT_TO_PTR(40));
  XTEST(node->next != NULL);
  XTEST(node->next->node == INT_TO_PTR(43));
  list_destroy(list);

  XPASS();
}
