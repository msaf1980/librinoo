/**
 * @file   list_popnode.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue May 11 10:02:07 2010
 *
 * @brief  list_popnode unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

int cmp_func(void *node1, void *node2)
{
	return node1 - node2;
}

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int main()
{
	t_list *list;
	t_listnode *tmp;
	t_listnode *tmp2;

	list = list_create(LIST_SORTED_HEAD, cmp_func);
	XTEST(list != NULL);
	tmp = list_add(list, INT_TO_PTR(42), NULL);
	XTEST(tmp != NULL);
	tmp = list_add(list, INT_TO_PTR(45), NULL);
	XTEST(tmp != NULL);
	tmp2 = list_add(list, INT_TO_PTR(43), NULL);
	XTEST(tmp != NULL);
	tmp = list_add(list, INT_TO_PTR(46), NULL);
	XTEST(tmp != NULL);
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->size == 4);
	XTEST(list->head->node == INT_TO_PTR(42));
	XTEST(list->tail->node == INT_TO_PTR(46));
	XTEST(list_popnode(list, tmp2) == 0);
	XTEST(tmp2->node == INT_TO_PTR(43));
	XTEST(list->size == 3);
	XTEST(list->head->node == INT_TO_PTR(42));
	XTEST(list->tail->node == INT_TO_PTR(46));
	XTEST(list_popnode(list, tmp) == 0);
	XTEST(tmp->node == INT_TO_PTR(46));
	XTEST(list->size == 2);
	XTEST(list->head->node == INT_TO_PTR(42));
	XTEST(list->tail->node == INT_TO_PTR(45));
	/* Inserting nodes again to free theses nodes */
	XTEST(list_addnode(list, tmp) == 0);
	XTEST(list_addnode(list, tmp2) == 0);
	list_destroy(list);

	list = list_create(LIST_SORTED_TAIL, cmp_func);
	XTEST(list != NULL);
	tmp = list_add(list, INT_TO_PTR(42), NULL);
	XTEST(tmp != NULL);
	tmp = list_add(list, INT_TO_PTR(45), NULL);
	XTEST(tmp != NULL);
	tmp2 = list_add(list, INT_TO_PTR(43), NULL);
	XTEST(tmp != NULL);
	tmp = list_add(list, INT_TO_PTR(46), NULL);
	XTEST(tmp != NULL);
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->size == 4);
	XTEST(list->head->node == INT_TO_PTR(42));
	XTEST(list->tail->node == INT_TO_PTR(46));
	XTEST(list_popnode(list, tmp2) == 0);
	XTEST(tmp2->node == INT_TO_PTR(43));
	XTEST(list->size == 3);
	XTEST(list->head->node == INT_TO_PTR(42));
	XTEST(list->tail->node == INT_TO_PTR(46));
	XTEST(list_popnode(list, tmp) == 0);
	XTEST(tmp->node == INT_TO_PTR(46));
	XTEST(list->size == 2);
	XTEST(list->head->node == INT_TO_PTR(42));
	XTEST(list->tail->node == INT_TO_PTR(45));
	/* Inserting nodes again to free theses nodes */
	XTEST(list_addnode(list, tmp) == 0);
	XTEST(list_addnode(list, tmp2) == 0);
	list_destroy(list);
	XPASS();
}
