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
  u32		size;
  u32		hashsize;
  t_listtype	listtype;
  t_list	**table;
  u32		(*hash_func)(void *node);
}		t_hashtable;

typedef struct	s_hashiterator
{
  u32			hash;
  t_listiterator	list_iterator;
}		t_hashiterator;

t_hashtable	*hashtable_create(t_listtype listtype,
				  u32 hashsize,
				  u32 (*hash_func)(void *node),
				  int (*cmp_func)(void *node1, void *node2));
void		hashtable_destroy(void *ptr);
int		hashtable_add(t_hashtable *htab,
			      void *node,
			      void (*free_func)(void *node));
int		hashtable_remove(t_hashtable *htab, void *node, u32 needfree);
void		*hashtable_find(t_hashtable *htab, void *node);
void		*hashtable_getnext(t_hashtable *htab, t_hashiterator *iterator);

#endif		/* !RINOO_HASHTABLE_H_ */
