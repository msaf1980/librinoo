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
 * Adds an element to a list
 *
 * @param list Pointer to the list
 * @param node Pointer to the node to add
 */
void rinoolist_add(t_rinoolist *list, t_rinoolist_node *node)
{
	node->prev = NULL;
	node->next = list->head;
	if (list->head != NULL) {
		list->head->prev = node;
	} else {
		list->tail = node;
	}
	list->head = node;
	list->count++;
}

/**
 * Removes an element from a list
 *
 * @param list Pointer to the list
 * @param node Pointer to the element to remove
 */
void rinoolist_remove(t_rinoolist *list, t_rinoolist_node *node)
{
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
	list->count--;
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
