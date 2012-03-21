/**
 * @file   hashtable_getnext.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Sun Jan  9 01:56:31 2011
 *
 * @brief  hashtable_getnext unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

#define		HASH_SIZE	42

u32 hash_func(void *node)
{
	return (PTR_TO_INT(node) / 10);
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
	t_rinoohash *hashtable;
	t_rinoohash_iter iterator = { 0, 0 };

	hashtable = rinoohash(RINOOLIST_SORTED_HEAD, HASH_SIZE, hash_func, hash_cmp, NULL);
	XTEST(hashtable != NULL);
	XTEST(hashtable->hashsize == HASH_SIZE);
	XTEST(hashtable->hash_func == hash_func);
	XTEST(hashtable->table != NULL);
	XTEST(hashtable->size == 0);
	XTEST(rinoohash_add(hashtable, INT_TO_PTR(42)) != NULL);
	XTEST(rinoohash_add(hashtable, INT_TO_PTR(43)) != NULL);
	XTEST(rinoohash_add(hashtable, INT_TO_PTR(52)) != NULL);
	XTEST(rinoohash_add(hashtable, INT_TO_PTR(53)) != NULL);
	XTEST(rinoohash_add(hashtable, INT_TO_PTR(62)) != NULL);
	XTEST(rinoohash_add(hashtable, INT_TO_PTR(63)) != NULL);
	XTEST(hashtable->size == 6);
	/* Element are inserted in the beginning, have to check for newest first */
	XTEST(rinoohash_getnext(hashtable, &iterator) == INT_TO_PTR(42));
	XTEST(rinoohash_getnext(hashtable, &iterator) == INT_TO_PTR(43));
	XTEST(rinoohash_getnext(hashtable, &iterator) == INT_TO_PTR(52));
	XTEST(rinoohash_getnext(hashtable, &iterator) == INT_TO_PTR(53));
	XTEST(rinoohash_getnext(hashtable, &iterator) == INT_TO_PTR(62));
	XTEST(rinoohash_getnext(hashtable, &iterator) == INT_TO_PTR(63));
	XTEST(rinoohash_getnext(hashtable, &iterator) == NULL);
	rinoohash_destroy(hashtable);
	XPASS();
}
