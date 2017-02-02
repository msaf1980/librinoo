/**
 * @file   rn_list_put.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2014
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rinoo rn_list add unit test
 *
 *
 */

#include "rinoo/rinoo.h"

#define RN_LISTTEST_NB_ELEM 10000

typedef struct mytest
{
	int val;
	rn_list_node_t node;
} tmytest;

int cmp_func(rn_list_node_t *node1, rn_list_node_t *node2)
{
	tmytest *a = container_of(node1, tmytest, node);
	tmytest *b = container_of(node2, tmytest, node);

	return (a->val == b->val ? 0 : (a->val > b->val ? 1 : -1));
}

int main()
{
	int i;
	int min;
	int last;
	tmytest *head;
	rn_list_t mylist;
	rn_list_node_t *node;
	tmytest tab[RN_LISTTEST_NB_ELEM];

	XTEST(rn_list(&mylist, cmp_func) == 0);
	for (i = 0; i < RN_LISTTEST_NB_ELEM; i++) {
		tab[i].val = random();
		if (i == 0 || min > tab[i].val) {
			min = tab[i].val;
		}
		rn_list_put(&mylist, &tab[i].node);
		XTEST(mylist.head != NULL);
		head = container_of(mylist.head, tmytest, node);
		XTEST(head->val == min);
	}
	XTEST(rn_list_size(&mylist) == RN_LISTTEST_NB_ELEM);
	last = head->val;
	for (i = 0; i < RN_LISTTEST_NB_ELEM; i++) {
		node = rn_list_pop(&mylist);
		XTEST(node != NULL);
		head = container_of(node, tmytest, node);
		XTEST(head->val >= last);
		last = head->val;
	}
	XPASS();
}
