/**
 * @file   list_create.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Wed Apr 28 16:42:20 2010
 *
 * @brief  list_create unit test
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
	t_list *list;

	list = list_create(LIST_SORTED_HEAD, cmp_func);
	XTEST(list != NULL);
	XTEST(list->type == LIST_SORTED_HEAD);
	XTEST(list->head == NULL);
	XTEST(list->tail == NULL);
	XTEST(list->cmp_func == cmp_func);
	XTEST(list->size == 0);
	list_destroy(list);
	list = list_create(LIST_SORTED_TAIL, cmp_func);
	XTEST(list != NULL);
	XTEST(list->type == LIST_SORTED_TAIL);
	XTEST(list->head == NULL);
	XTEST(list->tail == NULL);
	XTEST(list->cmp_func == cmp_func);
	XTEST(list->size == 0);
	list_destroy(list);
	XPASS();
}
