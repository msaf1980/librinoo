/**
 * @file   hashtable.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Wed Apr 28 14:42:05 2010
 *
 * @brief  Functions to manage hash table structures.
 *
 *
 */

#include "rinoo/rinoo.h"

/**
 * Creates a new hash table.
 *
 * @param listtype Type of list to use.
 * @param hashsize Array size of the hash table.
 * @param hash_func Pointer to a hash function.
 * @param cmp_func Pointer to a compare function.
 * @param free_func Pointer to a free function.
 *
 * @return Pointer to the new hash table.
 */
t_rinoohash *rinoohash(t_rinoolist_type listtype, u32 hashsize, t_rinoohash_func hash_func, t_rinoolist_cmp cmp_func, t_rinoolist_free free_func)
{
	u32 i;
	t_rinoohash *new;

	XASSERT(hashsize > 0, NULL);
	XASSERT(hash_func != NULL, NULL);
	XASSERT(cmp_func != NULL, NULL);

	new = calloc(1, sizeof(*new));
	if (unlikely(new == NULL)) {
		return NULL;
	}
	new->listtype = listtype;
	new->hashsize = hashsize;
	new->hash_func = hash_func;
	new->table = calloc(hashsize, sizeof(*new->table));
	if (unlikely(new->table == NULL)) {
		free(new);
		return NULL;
	}
	for (i = 0; i < hashsize; i++) {
		new->table[i] = rinoolist(listtype, cmp_func, free_func);
	}
	return new;
}

/**
 * Destroys a hashtable and its elements.
 *
 * @param ptr Pointer to the hashtable to destroy.
 */
void rinoohash_destroy(void *ptr)
{
	u32 i;
	t_rinoohash *htab;

	XASSERTN(ptr != NULL);

	htab = ptr;
	for (i = 0; i < htab->hashsize; i++) {
		rinoolist_destroy(htab->table[i]);
	}
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
int rinoohash_addnode(t_rinoohash *htab, t_rinoolist_node *node)
{
	XASSERT(htab != NULL, FALSE);

	if (unlikely(rinoolist_addnode(htab->table[htab->hash_func(node->ptr) % htab->hashsize], node) != 0)) {
		return -1;
	}
	htab->size++;
	return 0;
}

/**
 * Adds an element to a hash table.
 *
 * @param htab Pointer to the hash table where to add the element.
 * @param ptr Pointer to the element to add.
 *
 * @return A pointer to the new list node.
 */
t_rinoolist_node *rinoohash_add(t_rinoohash *htab, void *ptr)
{
	t_rinoolist_node *listnode;

	XASSERT(htab != NULL, FALSE);

	listnode = rinoolist_add(htab->table[htab->hash_func(ptr) % htab->hashsize], ptr);
	if (unlikely(listnode == NULL)) {
		return NULL;
	}
	htab->size++;
	return listnode;
}

/**
 * Deletes an element from a hash table.
 *
 * @param htab Pointer to the hash table to use.
 * @param ptr Pointer to the element to remove.
 * @param needfree Boolean which indicates if the node needs to be freed.
 *
 * @return 1 on success, 0 if an error occurs.
 */
int rinoohash_remove(t_rinoohash *htab, void *ptr, u32 needfree)
{
	int res;

	XASSERT(htab != NULL, FALSE);

	res = rinoolist_remove(htab->table[htab->hash_func(ptr) % htab->hashsize], ptr, needfree);
	if (res == TRUE) {
		htab->size--;
	}
	return res;
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
int rinoohash_removenode(t_rinoohash *htab, t_rinoolist_node *node, u32 needfree)
{
	int res;

	XASSERT(htab != NULL, FALSE);
	XASSERT(node != NULL, FALSE);

	res = rinoolist_removenode(htab->table[htab->hash_func(node->ptr) % htab->hashsize], node, needfree);
	if (res == TRUE) {
		htab->size--;
	}
	return res;
}

/**
 * Finds an element in a hash table.
 *
 * @param htab Pointer to the hash table to use.
 * @param ptr Pointer to an element to look for.
 *
 * @return Pointer to the element found or NULL if nothing is found.
 */
void *rinoohash_find(t_rinoohash *htab, void *ptr)
{
	XASSERT(htab != NULL, NULL);

	return rinoolist_find(htab->table[htab->hash_func(ptr) % htab->hashsize], ptr);
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
void *rinoohash_getnext(t_rinoohash *htab, t_rinoohash_iter *iterator)
{
	u32 i;
	void *result;

	XASSERT(htab != NULL, NULL);
	XASSERT(iterator != NULL, NULL);

	for (i = iterator->hash; i < htab->hashsize; i++) {
		iterator->hash = i;
		result = rinoolist_getnext(htab->table[i], &iterator->list_iterator);
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
int rinoohash_popnode(t_rinoohash *htab, t_rinoolist_node *node)
{
	XASSERT(htab != NULL, -1);
	XASSERT(node != NULL, -1);

	if (unlikely(rinoolist_popnode(htab->table[htab->hash_func(node->ptr) % htab->hashsize], node) != 0)) {
		return -1;
	}
	htab->size--;
	return 0;
}
