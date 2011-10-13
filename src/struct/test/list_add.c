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
	t_list *list;
	t_list *list2;
	t_list *list3;

	/* LIST_SORTED_HEAD test */
	list = list_create(LIST_SORTED_HEAD, cmp_func);
	XTEST(list != NULL);
	XTEST(list_add(list, INT_TO_PTR(42), NULL) != NULL);
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->head == list->tail);
	XTEST(list->size == 1);
	XTEST(list->head->node == INT_TO_PTR(42));
	XTEST(list_add(list, INT_TO_PTR(43), NULL) != NULL);
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->head != list->tail);
	XTEST(list->size == 2);
	XTEST(list->head->node == INT_TO_PTR(42));
	XTEST(list->tail->node == INT_TO_PTR(43));
	XTEST(list_add(list, INT_TO_PTR(44), NULL) != NULL);
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->head != list->tail);
	XTEST(list->size == 3);
	XTEST(list->head->node == INT_TO_PTR(42));
	XTEST(list->tail->node == INT_TO_PTR(44));
	XTEST(list_add(list, INT_TO_PTR(45), NULL) != NULL);
	XTEST(list->head != NULL);
	XTEST(list->tail != NULL);
	XTEST(list->head != list->tail);
	XTEST(list->size == 4);
	XTEST(list->head->node == INT_TO_PTR(42));
	XTEST(list->tail->node == INT_TO_PTR(45));

	/* LIST_SORTED_TAIL test */
	list2 = list_create(LIST_SORTED_TAIL, cmp_func);
	XTEST(list2 != NULL);
	XTEST(list_add(list2, INT_TO_PTR(42), NULL) != NULL);
	XTEST(list2->head != NULL);
	XTEST(list2->tail != NULL);
	XTEST(list2->head == list2->tail);
	XTEST(list2->size == 1);
	XTEST(list2->head->node == INT_TO_PTR(42));
	XTEST(list_add(list2, INT_TO_PTR(43), NULL) != NULL);
	XTEST(list2->head != NULL);
	XTEST(list2->tail != NULL);
	XTEST(list2->head != list2->tail);
	XTEST(list2->size == 2);
	XTEST(list2->head->node == INT_TO_PTR(42));
	XTEST(list2->tail->node == INT_TO_PTR(43));
	XTEST(list_add(list2, INT_TO_PTR(44), NULL) != NULL);
	XTEST(list2->head != NULL);
	XTEST(list2->tail != NULL);
	XTEST(list2->head != list2->tail);
	XTEST(list2->size == 3);
	XTEST(list2->head->node == INT_TO_PTR(42));
	XTEST(list2->tail->node == INT_TO_PTR(44));
	XTEST(list_add(list2, INT_TO_PTR(45), NULL) != NULL);
	XTEST(list2->head != NULL);
	XTEST(list2->tail != NULL);
	XTEST(list2->head != list2->tail);
	XTEST(list2->size == 4);
	XTEST(list2->head->node == INT_TO_PTR(42));
	XTEST(list2->tail->node == INT_TO_PTR(45));

	/* Destroys lists */
	list3 = list_create(LIST_SORTED_HEAD, cmp_func);
	XTEST(list_add(list3, list, list_destroy) != NULL);
	XTEST(list_add(list3, list2, list_destroy) != NULL);
	list_destroy(list3);
	XPASS();
}
