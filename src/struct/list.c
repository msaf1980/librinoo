/**
 * @file   list.c
 * @author reginaldl <reginald.@gmail.com> - Copyright 2013
 * @date   Sun Mar 24 23:21:28 2013
 *
 * @brief  LIST functions
 *
 *
 */

#include "rinoo/rinoo.h"

/**
 * Initializes a list.
 * If a compare function is provided, the list will be sorted.
 *
 * @param cmp Optional compare function.
 *
 * @return 0 on success otherwise -1
 */
int rinoolist(t_rinoolist *list, int (*cmp)(t_rinoolist_node *node1, t_rinoolist_node *node2))
{
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
	list->cmp = cmp;
	return 0;
}

/**
 * Flushes list content.
 *
 * @param list List to flush
 * @param delete Optional delete function to be called for each list node
 */
void rinoolist_flush(t_rinoolist *list, void (*delete)(t_rinoolist_node *node1))
{
	t_rinoolist_node *node;

	if (delete != NULL) {
		while ((node = rinoolist_pop(list)) != NULL) {
			delete(node);
		}
	}
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
}

/**
 * Gets list size.
 *
 * @param list List to use
 *
 * @return List size
 */
size_t rinoolist_size(t_rinoolist *list)
{
	return list->size;
}

/**
 * Adds an element to a list
 *
 * @param list Pointer to the list
 * @param node Pointer to the node to add
 */
void rinoolist_put(t_rinoolist *list, t_rinoolist_node *node)
{
	node->prev = NULL;
	node->next = list->head;
	if (list->cmp != NULL) {
		while (node->next != NULL && list->cmp(node->next, node) < 0) {
			node->prev = node->next;
			node->next = node->next->next;
		}
	}
	if (node->prev == NULL) {
		list->head = node;
	} else {
		node->prev->next = node;
	}
	if (node->next == NULL) {
		list->tail = node;
	} else {
		node->next->prev = node;
	}
	list->size++;
}

/**
 * Gets a node from a list.
 *
 * @param list List to use
 * @param node Dummy node used for comparison
 *
 * @return Pointer to the matching node or NULL if not found.
 */
t_rinoolist_node *rinoolist_get(t_rinoolist *list, t_rinoolist_node *node)
{
	if (list->cmp == NULL) {
		return NULL;
	}
	node->next = list->head;
	while (node->next != NULL) {
		if (list->cmp(node->next, node) == 0) {
			return node->next;
		}
		node->next = node->next->next;
	}
	return NULL;
}

/**
 * Removes an element from a list
 *
 * @param list Pointer to the list
 * @param node Pointer to the element to remove
 *
 * @return 0 on success otherwise -1 if an error occurs
 */
int rinoolist_remove(t_rinoolist *list, t_rinoolist_node *node)
{
	if (node->prev == NULL && node->next == NULL && list->head != node) {
		/* Node already removed */
		return -1;
	}
	if (node->prev != NULL) {
		node->prev->next = node->next;
	} else {
		list->head = node->next;
	}
	if (node->next != NULL) {
		node->next->prev = node->prev;
	} else {
		list->tail = node->prev;
	}
	list->size--;
	node->prev = NULL;
	node->next = NULL;
	return 0;
}

/**
 * Pops an element from a list
 *
 * @param list Pointer to the list
 *
 * @return First element in the list or NULL if empty
 */
t_rinoolist_node *rinoolist_pop(t_rinoolist *list)
{
	t_rinoolist_node *node;

	node = list->head;
	if (node != NULL) {
		rinoolist_remove(list, node);
	}
	return node;
}
