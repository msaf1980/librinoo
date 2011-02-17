/**
 * @file   hashtable_getnext.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan  9 01:56:31 2011
 *
 * @brief  hashtable_getnext unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

#define		HASH_SIZE	42

u32		hash_func(void *node)
{
  return (PTR_TO_INT(node) / 10);
}

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int		main()
{
  t_hashtable	*hashtable;
  t_hashiterator	iterator = { 0, 0 };

  hashtable = hashtable_create(LIST_SORTED_HEAD, HASH_SIZE, hash_func, NULL);
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
  /* Element are inserted in the beginning, have to check for newest first */
  XTEST(hashtable_getnext(hashtable, &iterator) == INT_TO_PTR(43));
  XTEST(hashtable_getnext(hashtable, &iterator) == INT_TO_PTR(42));
  XTEST(hashtable_getnext(hashtable, &iterator) == INT_TO_PTR(53));
  XTEST(hashtable_getnext(hashtable, &iterator) == INT_TO_PTR(52));
  XTEST(hashtable_getnext(hashtable, &iterator) == INT_TO_PTR(63));
  XTEST(hashtable_getnext(hashtable, &iterator) == INT_TO_PTR(62));
  XTEST(hashtable_getnext(hashtable, &iterator) == NULL);
  hashtable_destroy(hashtable);
  XPASS();
}
