/**
 * @file   list.c
 * @author reginaldl <reginald.@gmail.com> - Copyright 2014
 * @date   Thu Jul 31 22:44:54 2014
 *
 * @brief  Hash table functions
 *
 *
 */

#include "rinoo/struct/module.h"

/**
 * Initializes a hash table.
 *
 * @param htable Hash table initialize
 * @param size Hash table size
 * @param hash Hash function
 * @param compare Hash table node compare function
 *
 * @return 0 on success otherwise -1;
 */
int rinoohtable(t_rinoohtable *htable, size_t size, uint32_t (*hash)(t_rinoohtable_node *node), int (*compare)(t_rinoohtable_node *node1, t_rinoohtable_node *node2))
{
	htable->size = 0;
	htable->table_size = size;
	htable->hash = hash;
	htable->compare = compare;
	htable->table = calloc(size, sizeof(*htable->table));
	if (htable->table == NULL) {
		return -1;
	}
	return 0;
}

/**
 * Free allocated memory inside a hash table.
 *
 * @param htable Hash table to destroy
 */
void rinoohtable_destroy(t_rinoohtable *htable)
{
	if (htable->table != NULL) {
		free(htable->table);
		htable->table = NULL;
	}
}

/**
 * Flushes hash table content.
 *
 * @param htable Hash table to flush
 * @param delete Optional delete function to be called for each hash table node
 */
void rinoohtable_flush(t_rinoohtable *htable, void (*delete)(t_rinoohtable_node *node1))
{
	size_t i;
	t_rinoohtable_node *node;
	t_rinoohtable_node *next;

	if (delete != NULL) {
		for (i = 0; i < htable->table_size; i++) {
			node = htable->table[i];
			while (node != NULL) {
				next = node->next;
				delete(node);
				node = next;
			}
			htable->table[i] = NULL;
		}
	}
	htable->size = 0;
}

/**
 * Gets hash table size.
 *
 * @param htable Hash table to use
 *
 * @return Hash table size
 */
size_t rinoohtable_size(t_rinoohtable *htable)
{
	return htable->size;
}

/**
 * Adds an element to a hash table.
 *
 * @param htable Hash table to add to
 * @param node Hash table node to add
 */
void rinoohtable_put(t_rinoohtable *htable, t_rinoohtable_node *node)
{
	t_rinoohtable_node **head;

	node->hash = htable->hash(node);
	head = &htable->table[node->hash % htable->table_size];
	node->prev = NULL;
	node->next = *head;
	while (node->next != NULL && htable->compare(node->next, node) < 0) {
		node->prev = node->next;
		node->next = node->next->next;
	}
	if (node->prev == NULL) {
		*head = node;
	} else {
		node->prev->next = node;
	}
	if (node->next != NULL) {
		node->next->prev = node;
	}
	htable->size++;
}

/**
 * Gets a node from a hash table.
 *
 * @param htable Hash table to use
 * @param node Dummy node used for comparison
 *
 * @return The matching node or NULL if not found.
 */
t_rinoohtable_node *rinoohtable_get(t_rinoohtable *htable, t_rinoohtable_node *node)
{
	node->hash = htable->hash(node);
	node->next = htable->table[node->hash % htable->table_size];
	while (node->next != NULL) {
		if (htable->compare(node->next, node) == 0) {
			return node->next;
		}
		node->next = node->next->next;
	}
	return NULL;
}

/**
 * Removes an element from a hash table.
 *
 * @param htable Hash table to use
 * @param node Hash table node to remove
 *
 * @return 0 on success, otherwise -1
 */
int rinoohtable_remove(t_rinoohtable *htable, t_rinoohtable_node *node)
{
	if (node->prev != NULL) {
		node->prev->next = node->next;
	} else {
		htable->table[node->hash % htable->table_size] = node->next;
	}
	if (node->next != NULL) {
		node->next->prev = node->prev;
	}
	htable->size--;
	node->prev = NULL;
	node->next = NULL;
	return 0;
}
