/**
 * @file   hashtable.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Wed Apr 28 14:42:05 2010
 *
 * @brief  Functions to manage hash table structures.
 *
 *
 */

#include	"rinoo/rinoo.h"

/**
 * Creates a new hash table.
 *
 * @param listtype Type of list to use.
 * @param hashsize Array size of the hash table.
 * @param hash_func Pointer to a hash function.
 *
 * @return Pointer to the new hash table.
 */
t_hashtable *hashtable_create(t_listtype listtype,
			      u32 hashsize,
			      u32(*hash_func) (void *node),
			      int (*cmp_func) (void *node1, void *node2))
{
	u32 i;
	t_hashtable *new;

	XASSERT(hashsize > 0, NULL);
	XASSERT(hash_func != NULL, NULL);
	XASSERT(cmp_func != NULL, NULL);

	new = calloc(1, sizeof(*new));
	XASSERT(new != NULL, NULL);
	new->listtype = listtype;
	new->hashsize = hashsize;
	new->hash_func = hash_func;
	new->table = calloc(hashsize, sizeof(*new->table));
	if (new->table == NULL) {
		free(new);
		XASSERT(0, NULL);
	}
	for (i = 0; i < hashsize; i++) {
		new->table[i] = list_create(listtype, cmp_func);
	}
	return new;
}

/**
 * Destroys a hashtable and its elements.
 *
 * @param ptr Pointer to the hashtable to destroy.
 */
void hashtable_destroy(void *ptr)
{
	u32 i;
	t_hashtable *htab;

	XASSERTN(ptr != NULL);

	htab = (t_hashtable *) ptr;
	for (i = 0; i < htab->hashsize; i++)
		list_destroy(htab->table[i]);
	free(htab->table);
	free(htab);
}

/**
 * Adds an already existing list node to a hashtable.
 *
 * @param htab Pointer to the hashtable to use.
 * @param node Pointer to the new list node to add.
 *
 * @return 0 on success, -1 if an error occurs.
 */
int hashtable_addnode(t_hashtable * htab, t_listnode * node)
{
	XDASSERT(htab != NULL, FALSE);

	if (list_addnode
	    (htab->table[htab->hash_func(node->node) % htab->hashsize],
	     node) != 0) {
		return -1;
	}
	htab->size++;
	return 0;
}

/**
 * Adds an element to a hash table.
 *
 * @param htab Pointer to the hash table where to add the element.
 * @param node Pointer to the element to add.
 * @param free_func Pointer to a function which can free the element.
 *
 * @return A pointer to the new list node.
 */
t_listnode *hashtable_add(t_hashtable * htab,
			  void *node, void (*free_func) (void *node))
{
	t_listnode *listnode;
	XDASSERT(htab != NULL, FALSE);

	listnode = list_add(htab->table[htab->hash_func(node) % htab->hashsize],
			    node, free_func);
	if (listnode == NULL) {
		return NULL;
	}
	htab->size++;
	return listnode;
}

/**
 * Deletes an element from a hash table.
 *
 * @param htab Pointer to the hash table to use.
 * @param node Pointer to the element to remove.
 * @param needfree Boolean which indicates if the node needs to be freed.
 *
 * @return 1 on success, 0 if an error occurs.
 */
int hashtable_remove(t_hashtable * htab, void *node, u32 needfree)
{
	int res;

	XDASSERT(htab != NULL, FALSE);

	res = list_remove(htab->table[htab->hash_func(node) % htab->hashsize],
			  node, needfree);
	if (res == TRUE) {
		htab->size--;
	}
	return (res);
}

/**
 * Deletes a list node from the hashtable.
 *
 * @param htab Pointer to the hashtable to use.
 * @param node Pointer to the list node to remove.
 * @param needfree Boolean which indicates if the free_func has to be called.
 *
 * @return 1 on success, 0 if an error occurs.
 */
int hashtable_removenode(t_hashtable * htab, t_listnode * node, u32 needfree)
{
	int res;

	XDASSERT(htab != NULL, FALSE);
	XDASSERT(node != NULL, FALSE);

	res =
	    list_removenode(htab->
			    table[htab->hash_func(node->node) % htab->hashsize],
			    node, needfree);
	if (res == TRUE) {
		htab->size--;
	}
	return res;
}

/**
 * Finds an element in a hash table.
 *
 * @param htab Pointer to the hash table to use.
 * @param node Pointer to an element to look for.
 *
 * @return Pointer to the element found or NULL if nothing is found.
 */
void *hashtable_find(t_hashtable * htab, void *node)
{
	XDASSERT(htab != NULL, NULL);

	return (list_find
		(htab->table[htab->hash_func(node) % htab->hashsize], node));
}

/**
 * Get next element of a hashtable. The hash iterator stores the current hash
 * and a listiterator to easily access to the next element in the next call.
 *
 * @param htab Pointer to the hashtable to use.
 * @param iterator Pointer to a hashtable iterator where to store the current hash & list iterator.
 *
 * @return A pointer to the current element or NULL if the end is reached.
 */
void *hashtable_getnext(t_hashtable * htab, t_hashiterator * iterator)
{
	u32 i;
	void *result;

	XDASSERT(htab != NULL, NULL);
	XDASSERT(iterator != NULL, NULL);

	for (i = iterator->hash; i < htab->hashsize; i++) {
		iterator->hash = i;
		result = list_getnext(htab->table[i], &iterator->list_iterator);
		if (result != NULL) {
			return result;
		}
	}

	iterator->hash = 0;
	iterator->list_iterator = NULL;
	return NULL;
}

/**
 * Gets a list node out of a hashtable.
 *
 * @param htab Pointer to the hashtable to use.
 * @param node Pointer to the list node to pop.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int hashtable_popnode(t_hashtable * htab, t_listnode * node)
{
	XDASSERT(htab != NULL, -1);
	XDASSERT(node != NULL, -1);

	if (unlikely
	    (list_popnode
	     (htab->table[htab->hash_func(node->node) % htab->hashsize],
	      node) != 0)) {
		return -1;
	}
	htab->size--;
	return 0;
}
