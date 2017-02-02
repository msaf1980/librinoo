/**
 * @file   rn_list.c
 * @author Reginald Lips <reginald.@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  LIST functions
 *
 *
 */

#include "rinoo/struct/module.h"

/**
 * Initializes a rn_list.
 * If a compare function is provided, the rn_list will be sorted.
 *
 * @param rn_list List to initialize
 * @param compare Optional compare function
 *
 * @return 0 on success otherwise -1
 */
int rn_list(rn_list_t *rn_list, int (*compare)(rn_list_node_t *node1, rn_list_node_t *node2))
{
	rn_list->head = NULL;
	rn_list->tail = NULL;
	rn_list->size = 0;
	rn_list->compare = compare;
	return 0;
}

/**
 * Flushes rn_list content.
 *
 * @param rn_list List to flush
 * @param delete Optional delete function to be called for each rn_list node
 */
void rn_list_flush(rn_list_t *rn_list, void (*delete)(rn_list_node_t *node1))
{
	rn_list_node_t *node;

	if (delete != NULL) {
		while ((node = rn_list_pop(rn_list)) != NULL) {
			delete(node);
		}
	}
	rn_list->head = NULL;
	rn_list->tail = NULL;
	rn_list->size = 0;
}

/**
 * Gets rn_list size.
 *
 * @param rn_list List to use
 *
 * @return List size
 */
size_t rn_list_size(rn_list_t *rn_list)
{
	return rn_list->size;
}

/**
 * Adds an element to a rn_list
 *
 * @param rn_list Pointer to the rn_list
 * @param node Pointer to the node to add
 */
void rn_list_put(rn_list_t *rn_list, rn_list_node_t *node)
{
	node->prev = NULL;
	node->next = rn_list->head;
	if (rn_list->compare != NULL) {
		while (node->next != NULL && rn_list->compare(node->next, node) < 0) {
			node->prev = node->next;
			node->next = node->next->next;
		}
	}
	if (node->prev == NULL) {
		rn_list->head = node;
	} else {
		node->prev->next = node;
	}
	if (node->next == NULL) {
		rn_list->tail = node;
	} else {
		node->next->prev = node;
	}
	rn_list->size++;
}

/**
 * Gets a node from a rn_list.
 *
 * @param rn_list List to use
 * @param node Dummy node used for comparison
 *
 * @return Pointer to the matching node or NULL if not found.
 */
rn_list_node_t *rn_list_get(rn_list_t *rn_list, rn_list_node_t *node)
{
	if (rn_list->compare == NULL) {
		return NULL;
	}
	node->next = rn_list->head;
	while (node->next != NULL) {
		if (rn_list->compare(node->next, node) == 0) {
			return node->next;
		}
		node->next = node->next->next;
	}
	return NULL;
}

/**
 * Removes an element from a rn_list
 *
 * @param rn_list Pointer to the rn_list
 * @param node Pointer to the element to remove
 *
 * @return 0 on success otherwise -1 if an error occurs
 */
int rn_list_remove(rn_list_t *rn_list, rn_list_node_t *node)
{
	if (node->prev == NULL && node->next == NULL && rn_list->head != node) {
		/* Node already removed */
		return -1;
	}
	if (node->prev != NULL) {
		node->prev->next = node->next;
	} else {
		rn_list->head = node->next;
	}
	if (node->next != NULL) {
		node->next->prev = node->prev;
	} else {
		rn_list->tail = node->prev;
	}
	rn_list->size--;
	node->prev = NULL;
	node->next = NULL;
	return 0;
}

/**
 * Pops an element from a rn_list
 *
 * @param rn_list Pointer to the rn_list
 *
 * @return First element in the rn_list or NULL if empty
 */
rn_list_node_t *rn_list_pop(rn_list_t *rn_list)
{
	rn_list_node_t *node;

	node = rn_list->head;
	if (node != NULL) {
		rn_list_remove(rn_list, node);
	}
	return node;
}

/**
 * Returns rn_list's head
 *
 * @param rn_list Pointer to the rn_list
 *
 * @return First element in the rn_list or NULL if empty
 */
rn_list_node_t *rn_list_head(rn_list_t *rn_list)
{
	return rn_list->head;
}
