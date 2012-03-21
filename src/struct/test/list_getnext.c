/**
 * @file   list_getnext.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Thu Nov 25 16:50:14 2010
 *
 * @brief  list_getnext unit test
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
	t_rinoolist_iter iterator = NULL;

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
	XTEST(rinoolist_getnext(list, &iterator) == INT_TO_PTR(42));
	XTEST(iterator != NULL);
	XTEST(iterator->ptr == INT_TO_PTR(42));
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->size == 4);
	XTEST(rinoolist_getnext(list, &iterator) == INT_TO_PTR(43));
	XTEST(iterator != NULL);
	XTEST(iterator->ptr == INT_TO_PTR(43));
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->size == 4);
	XTEST(rinoolist_getnext(list, &iterator) == INT_TO_PTR(44));
	XTEST(iterator != NULL);
	XTEST(iterator->ptr == INT_TO_PTR(44));
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->size == 4);
	XTEST(rinoolist_getnext(list, &iterator) == INT_TO_PTR(45));
	XTEST(iterator != NULL);
	XTEST(iterator->ptr == INT_TO_PTR(45));
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->size == 4);
	XTEST(rinoolist_getnext(list, &iterator) == NULL);
	XTEST(iterator == NULL);
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->size == 4);
	rinoolist_destroy(list);
	XPASS();
}
