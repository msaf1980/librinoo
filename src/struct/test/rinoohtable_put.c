/**
 * @file   rinoohtable_put.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2014
 * @date   Thu Jul 31 22:46:42 2014
 *
 * @brief  rinoo htable add unit test
 *
 *
 */

#include "rinoo/rinoo.h"

#define RINOO_HTABLETEST_SIZE		42
#define RINOO_HTABLETEST_NB_ELEM	10000

typedef struct mytest
{
	int val;
	t_rinoohtable_node node;
} tmytest;

uint32_t hash_func(t_rinoohtable_node *node)
{
	tmytest *a = container_of(node, tmytest, node);

	return (uint32_t) a->val;
}

int cmp_func(t_rinoohtable_node *node1, t_rinoohtable_node *node2)
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
	t_rinoohtable htable;
	t_rinoohtable_node *node;
	tmytest tab[RINOO_HTABLETEST_NB_ELEM];

	XTEST(rinoohtable(&htable, 42, hash_func, cmp_func) == 0);
	for (i = 0; i < RINOO_HTABLETEST_NB_ELEM; i++) {
		tab[i].val = random();
		rinoohtable_put(&htable, &tab[i].node);
		XTEST(htable.table[tab[i].node.hash % htable.table_size] != NULL);
	}
	XTEST(rinoohtable_size(&htable) == RINOO_HTABLETEST_NB_ELEM);
	for (i = 0; i < RINOO_HTABLETEST_SIZE; i++) {
		node = htable.table[i];
		if (node != NULL) {
			head = container_of(node, tmytest, node);
			last = head->val;
		}
		while((node = htable.table[i]) != NULL) {
			head = container_of(node, tmytest, node);
			XTEST(head->val >= last);
			last = head->val;
			rinoohtable_remove(&htable, node);
		}
	}
	rinoohtable_destroy(&htable);
	XPASS();
}
