/**
 * @file   list_add.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Wed Apr 28 16:42:20 2010
 *
 * @brief  list_add unit test
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
	t_rinoolist *list2;
	t_rinoolist *list3;

	/* RINOOLIST_SORTED_HEAD test */
	list = rinoolist(RINOOLIST_SORTED_HEAD, cmp_func, NULL);
	XTEST(list != NULL);
	XTEST(rinoolist_add(list, INT_TO_PTR(42)) != NULL);
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->head == list->tail);
	XTEST(list->size == 1);
	XTEST(list->head->ptr == INT_TO_PTR(42));
	XTEST(rinoolist_add(list, INT_TO_PTR(43)) != NULL);
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->head != list->tail);
	XTEST(list->size == 2);
	XTEST(list->head->ptr == INT_TO_PTR(42));
	XTEST(list->tail->ptr == INT_TO_PTR(43));
	XTEST(rinoolist_add(list, INT_TO_PTR(44)) != NULL);
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->head != list->tail);
	XTEST(list->size == 3);
	XTEST(list->head->ptr == INT_TO_PTR(42));
	XTEST(list->tail->ptr == INT_TO_PTR(44));
	XTEST(rinoolist_add(list, INT_TO_PTR(45)) != NULL);
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->head != list->tail);
	XTEST(list->size == 4);
	XTEST(list->head->ptr == INT_TO_PTR(42));
	XTEST(list->tail->ptr == INT_TO_PTR(45));

	/* RINOOLIST_SORTED_TAIL test */
	list2 = rinoolist(RINOOLIST_SORTED_TAIL, cmp_func, NULL);
	XTEST(list2 != NULL);
	XTEST(rinoolist_add(list2, INT_TO_PTR(42)) != NULL);
	XTEST(list2->head != NULL);
	XTEST(list2->tail != NULL);
	XTEST(list2->head == list2->tail);
	XTEST(list2->size == 1);
	XTEST(list2->head->ptr == INT_TO_PTR(42));
	XTEST(rinoolist_add(list2, INT_TO_PTR(43)) != NULL);
	XTEST(list2->head != NULL);
	XTEST(list2->tail != NULL);
	XTEST(list2->head != list2->tail);
	XTEST(list2->size == 2);
	XTEST(list2->head->ptr == INT_TO_PTR(42));
	XTEST(list2->tail->ptr == INT_TO_PTR(43));
	XTEST(rinoolist_add(list2, INT_TO_PTR(44)) != NULL);
	XTEST(list2->head != NULL);
	XTEST(list2->tail != NULL);
	XTEST(list2->head != list2->tail);
	XTEST(list2->size == 3);
	XTEST(list2->head->ptr == INT_TO_PTR(42));
	XTEST(list2->tail->ptr == INT_TO_PTR(44));
	XTEST(rinoolist_add(list2, INT_TO_PTR(45)) != NULL);
	XTEST(list2->head != NULL);
	XTEST(list2->tail != NULL);
	XTEST(list2->head != list2->tail);
	XTEST(list2->size == 4);
	XTEST(list2->head->ptr == INT_TO_PTR(42));
	XTEST(list2->tail->ptr == INT_TO_PTR(45));

	/* Destroys lists */
	list3 = rinoolist(RINOOLIST_SORTED_HEAD, cmp_func, rinoolist_destroy);
	XTEST(rinoolist_add(list3, list) != NULL);
	XTEST(rinoolist_add(list3, list2) != NULL);
	rinoolist_destroy(list3);
	XPASS();
}
