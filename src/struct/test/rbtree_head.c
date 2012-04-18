/**
 * @file   rbtree_head.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Fri Apr 13 14:58:37 2012
 *
 * @brief  rinoo rbtree head unit test
 *
 *
 */

#include "rinoo/rinoo.h"

#define RINOO_HEADTEST_NB_ELEM		10000

typedef struct mytest
{
	int val;
	t_rinoorbtree_node node;
} tmytest;

int cmp_func(t_rinoorbtree_node *node1, t_rinoorbtree_node *node2)
{
	tmytest *a = container_of(node1, tmytest, node);
	tmytest *b = container_of(node2, tmytest, node);

	return (a->val == b->val ? 0 : (a->val > b->val ? 1 : -1));
}

int main()
{
	int i;
	int min;
	tmytest *head;
	tmytest tab[RINOO_HEADTEST_NB_ELEM];
	t_rinoorbtree tree;

	XTEST(rinoorbtree(&tree, cmp_func, NULL) == 0);
	for (i = 0; i < RINOO_HEADTEST_NB_ELEM; i++) {
		tab[i].val = random();
		if (i == 0 || min > tab[i].val) {
			min = tab[i].val;
		}
		XTEST(rinoorbtree_put(&tree, &tab[i].node) == 0);
		XTEST(tree.head != NULL);
		head = container_of(tree.head, tmytest, node);
		XTEST(head->val == min);
	}
	rinoorbtree_flush(&tree);
	XPASS();
}
