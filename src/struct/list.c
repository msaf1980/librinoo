/**
 * @file   list.c
 * @author reginaldl <reginald.@gmail.com>
 * @date   Sun Mar 24 23:21:28 2013
 *
 * @brief  LIST functions
 *
 *
 */

#include "rinoo/rinoo.h"

void rinoolist_add(t_rinoolist *list, t_rinoolist_node *node)
{
	node->next = list->head;
	list->head = node;
	if (list->tail == NULL) {
		list->tail = node;
	}
	list->count++;
}

void rinoolist_remove(t_rinoolist *list, t_rinoolist_node *node)
{
	if (node->prev != NULL) {
		node->prev->next = node->next;
	} else {
		list->head = node->next;
	}
	if (list->tail == node) {
		list->tail = NULL;
	}
	list->count--;
}

t_rinoolist_node *rinoolist_pop(t_rinoolist *list)
{
	t_rinoolist_node *node;

	node = list->head;
	if (node != NULL) {
		rinoolist_remove(list, node);
	}
	return node;
}
