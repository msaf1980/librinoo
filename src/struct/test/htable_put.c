/**
 * @file   rn_htable_put.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2014
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rinoo rn_htable add unit test
 *
 *
 */

#include "rinoo/rinoo.h"

#define RN_HTABLETEST_SIZE		42
#define RN_HTABLETEST_NB_ELEM	10000

typedef struct mytest
{
	int val;
	rn_htable_node_t node;
} tmytest;

uint32_t hash_func(rn_htable_node_t *node)
{
	tmytest *a = container_of(node, tmytest, node);

	return (uint32_t) a->val;
}

int cmp_func(rn_htable_node_t *node1, rn_htable_node_t *node2)
{
	tmytest *a = container_of(node1, tmytest, node);
	tmytest *b = container_of(node2, tmytest, node);

	return (a->val == b->val ? 0 : (a->val > b->val ? 1 : -1));
}

int main()
{
	int i;
	int last;
	tmytest *head;
	rn_htable_t htab;
	rn_htable_node_t *node;
	tmytest tab[RN_HTABLETEST_NB_ELEM];

	XTEST(rn_htable(&htab, 42, hash_func, cmp_func) == 0);
	for (i = 0; i < RN_HTABLETEST_NB_ELEM; i++) {
		tab[i].val = random();
		rn_htable_put(&htab, &tab[i].node);
		XTEST(htab.table[tab[i].node.hash % htab.table_size] != NULL);
	}
	XTEST(rn_htable_size(&htab) == RN_HTABLETEST_NB_ELEM);
	for (i = 0; i < RN_HTABLETEST_SIZE; i++) {
		node = htab.table[i];
		if (node != NULL) {
			head = container_of(node, tmytest, node);
			last = head->val;
		}
		while((node = htab.table[i]) != NULL) {
			head = container_of(node, tmytest, node);
			XTEST(head->val >= last);
			last = head->val;
			rn_htable_remove(&htab, node);
		}
	}
	rn_htable_destroy(&htab);
	XPASS();
}
