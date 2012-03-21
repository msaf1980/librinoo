/**
 * @file   hashtable.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Wed Apr 28 14:43:14 2010
 *
 * @brief  Header file for hash table functions declarations.
 *
 *
 */

#ifndef		RINOO_STRUCT_HASHTABLE_H_
# define	RINOO_STRUCT_HASHTABLE_H_

typedef u32	(*t_rinoohash_func)(void *ptr);

typedef struct s_rinoohash
{
	u32			size;
	u32			hashsize;
	t_rinoolist_type	listtype;
	t_rinoolist		**table;
	t_rinoohash_func	hash_func;
} t_rinoohash;

typedef struct s_rinoohash_iter
{
	u32			hash;
	t_rinoolist_iter	list_iterator;
} t_rinoohash_iter;

t_rinoohash *rinoohash(t_rinoolist_type listtype, u32 hashsize, t_rinoohash_func hash_func, t_rinoolist_cmp cmp_func, t_rinoolist_free free_func);
void rinoohash_destroy(void *ptr);
int rinoohash_addnode(t_rinoohash *htab, t_rinoolist_node *node);
t_rinoolist_node *rinoohash_add(t_rinoohash *htab, void *ptr);
int rinoohash_remove(t_rinoohash *htab, void *node, u32 needfree);
int rinoohash_removenode(t_rinoohash *htab, t_rinoolist_node *node, u32 needfree);
void *rinoohash_find(t_rinoohash *htab, void *ptr);
void *rinoohash_getnext(t_rinoohash *htab, t_rinoohash_iter *iterator);
int rinoohash_popnode(t_rinoohash *htab, t_rinoolist_node *node);

#endif		/* !RINOO_STRUCT_RINOOHASH_H_ */
