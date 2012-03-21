/**
 * @file   list_addnode.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Wed Apr 28 16:42:20 2010
 *
 * @brief  list_addnode unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

int cmp_func(void *unused(node1), void *unused(node2))
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
	t_rinoolist_node *node;

	/* RINOOLIST_SORTED_HEAD test */
	list = rinoolist(RINOOLIST_SORTED_HEAD, cmp_func, NULL);
	XTEST(list != NULL);
	node = rinoolist_add(list, INT_TO_PTR(42));
	XTEST(node != NULL);
	XTEST(rinoolist_popnode(list, node) == 0);
	XTEST(rinoolist_add(list, INT_TO_PTR(44)) != NULL);
	XTEST(rinoolist_addnode(list, node) == 0);
	XTEST(node->prev == NULL);
	XTEST(node->next != NULL);
	XTEST(node->next->ptr == INT_TO_PTR(44));
	XTEST(rinoolist_popnode(list, node) == 0);
	XTEST(rinoolist_add(list, INT_TO_PTR(43)) != NULL);
	XTEST(rinoolist_addnode(list, node) == 0);
	XTEST(node->prev == NULL);
	XTEST(node->next != NULL);
	XTEST(node->next->ptr == INT_TO_PTR(43));
	XTEST(rinoolist_popnode(list, node) == 0);
	XTEST(rinoolist_add(list, INT_TO_PTR(40)) != NULL);
	XTEST(rinoolist_addnode(list, node) == 0);
	XTEST(node->prev != NULL);
	XTEST(node->prev->ptr == INT_TO_PTR(40));
	XTEST(node->next != NULL);
	XTEST(node->next->ptr == INT_TO_PTR(43));
	XTEST(rinoolist_popnode(list, node) == 0);
	XTEST(rinoolist_add(list, INT_TO_PTR(46)) != NULL);
	XTEST(rinoolist_addnode(list, node) == 0);
	XTEST(node->prev != NULL);
	XTEST(node->prev->ptr == INT_TO_PTR(40));
	XTEST(node->next != NULL);
	XTEST(node->next->ptr == INT_TO_PTR(43));
	XTEST(rinoolist_popnode(list, node) == 0);
	XTEST(rinoolist_add(list, INT_TO_PTR(45)) != NULL);
	XTEST(rinoolist_addnode(list, node) == 0);
	XTEST(node->prev != NULL);
	XTEST(node->prev->ptr == INT_TO_PTR(40));
	XTEST(node->next != NULL);
	XTEST(node->next->ptr == INT_TO_PTR(43));
	rinoolist_destroy(list);

	/* RINOOLIST_SORTED_TAIL test */
	list = rinoolist(RINOOLIST_SORTED_TAIL, cmp_func, NULL);
	XTEST(list != NULL);
	node = rinoolist_add(list, INT_TO_PTR(42));
	XTEST(node != NULL);
	XTEST(rinoolist_popnode(list, node) == 0);
	XTEST(rinoolist_add(list, INT_TO_PTR(44)) != NULL);
	XTEST(rinoolist_addnode(list, node) == 0);
	XTEST(node->prev == NULL);
	XTEST(node->next != NULL);
	XTEST(node->next->ptr == INT_TO_PTR(44));
	XTEST(rinoolist_popnode(list, node) == 0);
	XTEST(rinoolist_add(list, INT_TO_PTR(43)) != NULL);
	XTEST(rinoolist_addnode(list, node) == 0);
	XTEST(node->prev == NULL);
	XTEST(node->next != NULL);
	XTEST(node->next->ptr == INT_TO_PTR(43));
	XTEST(rinoolist_popnode(list, node) == 0);
	XTEST(rinoolist_add(list, INT_TO_PTR(40)) != NULL);
	XTEST(rinoolist_addnode(list, node) == 0);
	XTEST(node->prev != NULL);
	XTEST(node->prev->ptr == INT_TO_PTR(40));
	XTEST(node->next != NULL);
	XTEST(node->next->ptr == INT_TO_PTR(43));
	XTEST(rinoolist_popnode(list, node) == 0);
	XTEST(rinoolist_add(list, INT_TO_PTR(46)) != NULL);
	XTEST(rinoolist_addnode(list, node) == 0);
	XTEST(node->prev != NULL);
	XTEST(node->prev->ptr == INT_TO_PTR(40));
	XTEST(node->next != NULL);
	XTEST(node->next->ptr == INT_TO_PTR(43));
	XTEST(rinoolist_popnode(list, node) == 0);
	XTEST(rinoolist_add(list, INT_TO_PTR(45)) != NULL);
	XTEST(rinoolist_addnode(list, node) == 0);
	XTEST(node->prev != NULL);
	XTEST(node->prev->ptr == INT_TO_PTR(40));
	XTEST(node->next != NULL);
	XTEST(node->next->ptr == INT_TO_PTR(43));
	rinoolist_destroy(list);

	XPASS();
}
