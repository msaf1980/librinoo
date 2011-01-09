/**
 * @file   hashtable.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Apr 28 14:43:14 2010
 *
 * @brief  Header file for hash table functions declarations.
 *
 *
 */

#ifndef		RINOO_HASHTABLE_H_
# define	RINOO_HASHTABLE_H_

typedef struct	s_hashtable
{
  t_list	**table;
  u32		hashsize;
  u32		(*hash_func)(void *node);
  u32		size;
}		t_hashtable;

typedef struct	s_hashiterator
{
  u32			hash;
  t_listiterator	list_iterator;
}		t_hashiterator;

t_hashtable	*hashtable_create(u32 hashsize,
				  u32 (*hash_func)(),
				  int (*cmp_func)(void *node1, void *node2));
void		hashtable_destroy(void *ptr);
int		hashtable_add(t_hashtable *htab,
			      void *node,
			      void (*free_func)());
int		hashtable_remove(t_hashtable *htab, void *node, u32 needfree);
void		*hashtable_find(t_hashtable *htab, void *node);
void		*hashtable_getnext(t_hashtable *htab, t_hashiterator *iterator);

#endif		/* !RINOO_HASHTABLE_H_ */
