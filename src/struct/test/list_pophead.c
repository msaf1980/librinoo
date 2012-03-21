/**
 * @file   list_pophead.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue May 11 10:02:07 2010
 *
 * @brief  list_pophead unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

int cmp_func(void *unused(node1), void *unused(node2))
{
	return 1;
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

	list = rinoolist(RINOOLIST_SORTED_HEAD, cmp_func, NULL);
	XTEST(list != NULL);
	XTEST(rinoolist_add(list, INT_TO_PTR(42)) != NULL);
	XTEST(rinoolist_add(list, INT_TO_PTR(43)) != NULL);
	XTEST(rinoolist_add(list, INT_TO_PTR(44)) != NULL);
	XTEST(rinoolist_add(list, INT_TO_PTR(45)) != NULL);
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->size == 4);
	XTEST(list->head->ptr == INT_TO_PTR(42));
	XTEST(list->tail->ptr == INT_TO_PTR(45));
	XTEST(rinoolist_pophead(list) == INT_TO_PTR(42));
	XTEST(list->head != NULL);
	XTEST(list->head->ptr == INT_TO_PTR(43));
	XTEST(list->tail != NULL);
	XTEST(list->tail->ptr == INT_TO_PTR(45));
	XTEST(list->size == 3);
	XTEST(rinoolist_pophead(list) == INT_TO_PTR(43));
	XTEST(list->head != NULL);
	XTEST(list->head->ptr == INT_TO_PTR(44));
	XTEST(list->tail != NULL);
	XTEST(list->tail->ptr == INT_TO_PTR(45));
	XTEST(list->size == 2);
	XTEST(rinoolist_pophead(list) == INT_TO_PTR(44));
	XTEST(list->head != NULL);
	XTEST(list->head == list->tail);
	XTEST(list->head->ptr == INT_TO_PTR(45));
	XTEST(list->size == 1);
	XTEST(rinoolist_pophead(list) == INT_TO_PTR(45));
	XTEST(list->head == NULL);
	XTEST(list->tail == NULL);
	XTEST(list->size == 0);
	rinoolist_destroy(list);
	XPASS();
}
