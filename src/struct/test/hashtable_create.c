/**
 * @file   hashtable_create.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
 * @date   Wed Apr 28 16:42:20 2010
 *
 * @brief  hashtable_create unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

#define		HASH_SIZE	42

u32		hash_func(void *node)
{
  return (PTR_TO_INT(node) % HASH_SIZE);
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

  hashtable = hashtable_create(HASH_SIZE, hash_func, NULL);
  XTEST(hashtable != NULL);
  XTEST(hashtable->hashsize == HASH_SIZE);
  XTEST(hashtable->hash_func == hash_func);
  XTEST(hashtable->table != NULL);
  XTEST(hashtable->size == 0);
  hashtable_destroy(hashtable);
  XPASS();
}
