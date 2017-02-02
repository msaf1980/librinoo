/**
 * @file   rn_rbtree_head.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rinoo rn_rbtree head unit test
 *
 *
 */

#include "rinoo/rinoo.h"

#define RN_HEADTEST_NB_ELEM		10000

typedef struct mytest
{
	int val;
	rn_rbtree_node_t node;
} tmytest;

int cmp_func(rn_rbtree_node_t *node1, rn_rbtree_node_t *node2)
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
	rn_rbtree_t tree;
	rn_rbtree_node_t *node;
	tmytest tab[RN_HEADTEST_NB_ELEM];

	XTEST(rn_rbtree(&tree, cmp_func, NULL) == 0);
	for (i = 0; i < RN_HEADTEST_NB_ELEM; i++) {
		tab[i].val = random();
		if (i == 0 || min > tab[i].val) {
			min = tab[i].val;
		}
		XTEST(rn_rbtree_put(&tree, &tab[i].node) == 0);
		XTEST(tree.head != NULL);
		head = container_of(tree.head, tmytest, node);
		XTEST(head->val == min);
	}
	rn_rbtree_flush(&tree);
	for (i = 0; i < RN_HEADTEST_NB_ELEM; i++) {
		tab[i].val = i;
		XTEST(rn_rbtree_put(&tree, &tab[i].node) == 0);
		XTEST(tree.head != NULL);
	}
	for (i = 0; i < RN_HEADTEST_NB_ELEM; i++) {
		node = rn_rbtree_head(&tree);
		XTEST(node != NULL);
		head = container_of(node, tmytest, node);
		XTEST(head->val == i);
		rn_rbtree_remove(&tree, node);
	}
	XPASS();
}
