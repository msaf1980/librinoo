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
	t_rinoolist *list;
	t_rinoolist_node *tmp;
	t_rinoolist_node *tmp2;

	list = rinoolist(RINOOLIST_SORTED_HEAD, cmp_func, NULL);
	XTEST(list != NULL);
	tmp = rinoolist_add(list, INT_TO_PTR(42));
	XTEST(tmp != NULL);
	tmp = rinoolist_add(list, INT_TO_PTR(45));
	XTEST(tmp != NULL);
	tmp2 = rinoolist_add(list, INT_TO_PTR(43));
	XTEST(tmp != NULL);
	tmp = rinoolist_add(list, INT_TO_PTR(46));
	XTEST(tmp != NULL);
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->size == 4);
	XTEST(list->head->ptr == INT_TO_PTR(42));
	XTEST(list->tail->ptr == INT_TO_PTR(46));
	XTEST(rinoolist_popnode(list, tmp2) == 0);
	XTEST(tmp2->ptr == INT_TO_PTR(43));
	XTEST(list->size == 3);
	XTEST(list->head->ptr == INT_TO_PTR(42));
	XTEST(list->tail->ptr == INT_TO_PTR(46));
	XTEST(rinoolist_popnode(list, tmp) == 0);
	XTEST(tmp->ptr == INT_TO_PTR(46));
	XTEST(list->size == 2);
	XTEST(list->head->ptr == INT_TO_PTR(42));
	XTEST(list->tail->ptr == INT_TO_PTR(45));
	/* Inserting nodes again to free theses nodes */
	XTEST(rinoolist_addnode(list, tmp) == 0);
	XTEST(rinoolist_addnode(list, tmp2) == 0);
	rinoolist_destroy(list);

	list = rinoolist(RINOOLIST_SORTED_TAIL, cmp_func, NULL);
	XTEST(list != NULL);
	tmp = rinoolist_add(list, INT_TO_PTR(42));
	XTEST(tmp != NULL);
	tmp = rinoolist_add(list, INT_TO_PTR(45));
	XTEST(tmp != NULL);
	tmp2 = rinoolist_add(list, INT_TO_PTR(43));
	XTEST(tmp != NULL);
	tmp = rinoolist_add(list, INT_TO_PTR(46));
	XTEST(tmp != NULL);
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->size == 4);
	XTEST(list->head->ptr == INT_TO_PTR(42));
	XTEST(list->tail->ptr == INT_TO_PTR(46));
	XTEST(rinoolist_popnode(list, tmp2) == 0);
	XTEST(tmp2->ptr == INT_TO_PTR(43));
	XTEST(list->size == 3);
	XTEST(list->head->ptr == INT_TO_PTR(42));
	XTEST(list->tail->ptr == INT_TO_PTR(46));
	XTEST(rinoolist_popnode(list, tmp) == 0);
	XTEST(tmp->ptr == INT_TO_PTR(46));
	XTEST(list->size == 2);
	XTEST(list->head->ptr == INT_TO_PTR(42));
	XTEST(list->tail->ptr == INT_TO_PTR(45));
	/* Inserting nodes again to free theses nodes */
	XTEST(rinoolist_addnode(list, tmp) == 0);
	XTEST(rinoolist_addnode(list, tmp2) == 0);
	rinoolist_destroy(list);
	XPASS();
}
