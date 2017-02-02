/**
 * @file   rn_list.c
 * @author Reginald Lips <reginald.@gmail.com> - Copyright 2014
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Hash table functions
 *
 *
 */

#include "rinoo/struct/module.h"

/**
 * Initializes a hash table.
 *
 * @param rn_htable Hash table initialize
 * @param size Hash table size
 * @param hash Hash function
 * @param compare Hash table node compare function
 *
 * @return 0 on success otherwise -1;
 */
int rn_htable(rn_htable_t *rn_htable, size_t size, uint32_t (*hash)(rn_htable_node_t *node), int (*compare)(rn_htable_node_t *node1, rn_htable_node_t *node2))
{
	rn_htable->size = 0;
	rn_htable->table_size = size;
	rn_htable->hash = hash;
	rn_htable->compare = compare;
	rn_htable->table = calloc(size, sizeof(*rn_htable->table));
	if (rn_htable->table == NULL) {
		return -1;
	}
	return 0;
}

/**
 * Free allocated memory inside a hash table.
 *
 * @param rn_htable Hash table to destroy
 */
void rn_htable_destroy(rn_htable_t *rn_htable)
{
	if (rn_htable->table != NULL) {
		free(rn_htable->table);
		rn_htable->table = NULL;
	}
}

/**
 * Flushes hash table content.
 *
 * @param rn_htable Hash table to flush
 * @param delete Optional delete function to be called for each hash table node
 */
void rn_htable_flush(rn_htable_t *rn_htable, void (*delete)(rn_htable_node_t *node1))
{
	size_t i;
	rn_htable_node_t *node;
	rn_htable_node_t *next;

	if (delete != NULL) {
		for (i = 0; i < rn_htable->table_size; i++) {
			node = rn_htable->table[i];
			while (node != NULL) {
				next = node->next;
				delete(node);
				node = next;
			}
			rn_htable->table[i] = NULL;
		}
	}
	rn_htable->size = 0;
}

/**
 * Gets hash table size.
 *
 * @param rn_htable Hash table to use
 *
 * @return Hash table size
 */
size_t rn_htable_size(rn_htable_t *rn_htable)
{
	return rn_htable->size;
}

/**
 * Adds an element to a hash table.
 *
 * @param rn_htable Hash table to add to
 * @param node Hash table node to add
 */
void rn_htable_put(rn_htable_t *rn_htable, rn_htable_node_t *node)
{
	rn_htable_node_t **head;

	node->hash = rn_htable->hash(node);
	head = &rn_htable->table[node->hash % rn_htable->table_size];
	node->prev = NULL;
	node->next = *head;
	while (node->next != NULL && rn_htable->compare(node->next, node) < 0) {
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
	rn_htable->size++;
}

/**
 * Gets a node from a hash table.
 *
 * @param rn_htable Hash table to use
 * @param node Dummy node used for comparison
 *
 * @return The matching node or NULL if not found.
 */
rn_htable_node_t *rn_htable_get(rn_htable_t *rn_htable, rn_htable_node_t *node)
{
	node->hash = rn_htable->hash(node);
	node->next = rn_htable->table[node->hash % rn_htable->table_size];
	while (node->next != NULL) {
		if (rn_htable->compare(node->next, node) == 0) {
			return node->next;
		}
		node->next = node->next->next;
	}
	return NULL;
}

/**
 * Removes an element from a hash table.
 *
 * @param rn_htable Hash table to use
 * @param node Hash table node to remove
 *
 * @return 0 on success, otherwise -1
 */
int rn_htable_remove(rn_htable_t *rn_htable, rn_htable_node_t *node)
{
	if (node->prev != NULL) {
		node->prev->next = node->next;
	} else {
		rn_htable->table[node->hash % rn_htable->table_size] = node->next;
	}
	if (node->next != NULL) {
		node->next->prev = node->prev;
	}
	rn_htable->size--;
	node->prev = NULL;
	node->next = NULL;
	return 0;
}
