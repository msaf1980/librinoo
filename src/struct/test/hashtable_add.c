/**
 * @file   hashtable_add.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Sun Jan  9 02:22:59 2011
 *
 * @brief  hashtable_add unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

#define		HASH_SIZE	42

u32 hash_func(void *node)
{
	return PTR_TO_INT(node) / 10;
}

int hash_cmp(void *node1, void *node2)
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
	t_hashtable *hashtable;

	hashtable =
	    hashtable_create(LIST_SORTED_HEAD, HASH_SIZE, hash_func, hash_cmp);
	XTEST(hashtable != NULL);
	XTEST(hashtable->hashsize == HASH_SIZE);
	XTEST(hashtable->hash_func == hash_func);
	XTEST(hashtable->table != NULL);
	XTEST(hashtable->size == 0);
	XTEST(hashtable_add(hashtable, INT_TO_PTR(42), NULL) != NULL);
	XTEST(hashtable_add(hashtable, INT_TO_PTR(43), NULL) != NULL);
	XTEST(hashtable_add(hashtable, INT_TO_PTR(52), NULL) != NULL);
	XTEST(hashtable_add(hashtable, INT_TO_PTR(53), NULL) != NULL);
	XTEST(hashtable_add(hashtable, INT_TO_PTR(62), NULL) != NULL);
	XTEST(hashtable_add(hashtable, INT_TO_PTR(63), NULL) != NULL);
	XTEST(hashtable->size == 6);
	XTEST(hashtable->table[4] != NULL);
	XTEST(hashtable->table[4]->head != NULL);
	XTEST(hashtable->table[4]->tail != NULL);
	XTEST(hashtable->table[4]->head->node == INT_TO_PTR(42));
	XTEST(hashtable->table[4]->tail->node == INT_TO_PTR(43));
	XTEST(hashtable->table[5] != NULL);
	XTEST(hashtable->table[5]->head != NULL);
	XTEST(hashtable->table[5]->tail != NULL);
	XTEST(hashtable->table[5]->head->node == INT_TO_PTR(52));
	XTEST(hashtable->table[5]->tail->node == INT_TO_PTR(53));
	XTEST(hashtable->table[6] != NULL);
	XTEST(hashtable->table[6]->head != NULL);
	XTEST(hashtable->table[6]->tail != NULL);
	XTEST(hashtable->table[6]->head->node == INT_TO_PTR(62));
	XTEST(hashtable->table[6]->tail->node == INT_TO_PTR(63));
	hashtable_destroy(hashtable);
	XPASS();
}
