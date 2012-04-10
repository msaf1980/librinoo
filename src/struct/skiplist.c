/**
 * @file   skiplist.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Fri Mar 16 15:38:21 2012
 *
 * @brief  Functions to manage skip list structures
 *
 *
 */

#include	"rinoo/rinoo.h"

t_rinooskip *rinooskip(t_rinoolist_cmp cmp_func, t_rinoolist_free free_func)
{
	t_rinooskip *new;

	XASSERT(cmp_func != NULL, NULL);

	new = calloc(1, sizeof(*new));
	if (unlikely(new == NULL)) {
		return NULL;
	}
	new->head.forward = calloc(RINOO_SKIPLIST_MAXLEVEL, sizeof(*new->head.forward));
	if (unlikely(new->head.forward == NULL)) {
		free(new);
		return NULL;
	}
	new->cmp_func = cmp_func;
	new->free_func = free_func;
	return new;
}

void rinooskip_destroy(t_rinooskip *list)
{
	t_rinooskip_node *cur;
	t_rinooskip_node *next;

	XASSERTN(list != NULL);

	cur = list->head.forward[0];
	while (cur != NULL) {
		next = cur->forward[0];
		free(cur->forward);
		free(cur);
		cur = next;
	}
	free(list->head.forward);
	free(list);
}

int rinooskip_add(t_rinooskip *list, void *ptr)
{
	int i;
	int new_level;
	t_rinooskip_node *newnode;
	t_rinooskip_node *curnode;
	t_rinooskip_node *update[RINOO_SKIPLIST_MAXLEVEL];

	curnode = &list->head;
	for (i = list->level; i >= 0; i--) {
		while (curnode->forward[i] != NULL &&
		       list->cmp_func(curnode->forward[i]->ptr, ptr) < 0) {
			curnode = curnode->forward[i];
		}
		update[i] = curnode;
	}
        for (new_level = 0; random() < (RAND_MAX / 2) && new_level < RINOO_SKIPLIST_MAXLEVEL - 1; new_level++);
	newnode = malloc(sizeof(*newnode));
	if (unlikely(newnode == NULL)) {
		return -1;
	}
	newnode->forward = calloc(new_level + 1, sizeof(*newnode->forward));
	if (unlikely(newnode->forward == NULL)) {
		free(newnode);
		return -1;
	}
	newnode->level = new_level;
	newnode->ptr = ptr;
	for (i = 0; i <= new_level; i++) {
		if (i <= list->level) {
			newnode->forward[i] = update[i]->forward[i];
			update[i]->forward[i] = newnode;
		} else {
			newnode->forward[i] = NULL;
			list->head.forward[i] = newnode;
		}
	}
	if (list->level < new_level) {
		list->level = new_level;
	}
	return 0;
}

void *rinooskip_pop(t_rinooskip *list)
{
	int i;
	void *result;
	t_rinooskip_node *node;

	node = list->head.forward[0];
	if (node == NULL) {
		return NULL;
	}
	for (i = 0; i <= list->level && list->head.forward[i] == node; i++) {
		list->head.forward[i] = node->forward[i];
	}
	result = node->ptr;
	free(node->forward);
	free(node);
	return result;
}

void *rinooskip_head(t_rinooskip *list)
{
	t_rinooskip_node *node;

	node = list->head.forward[0];
	if (node == NULL) {
		return NULL;
	}
	return node->ptr;
}

void *rinooskip_remove(t_rinooskip *list, void *ptr)
{
	int i;
	int ret;
	t_rinooskip_node *curnode;
	t_rinooskip_node *update[RINOO_SKIPLIST_MAXLEVEL];

	curnode = &list->head;
	for (i = list->level; i >= 0; i--) {
		while (curnode->forward[i] != NULL &&
		       (ret = list->cmp_func(curnode->forward[i]->ptr, ptr)) < 0) {
			curnode = curnode->forward[i];
		}
		update[i] = curnode;
	}
	if (curnode == NULL || ret != 0) {
		return NULL;
	}
	for (i = 0; i <= curnode->level; i++) {
		if (i <= list->level) {
			newnode->forward[i] = update[i]->forward[i];
			update[i]->forward[i] = newnode;
		} else {
			newnode->forward[i] = NULL;
			list->head.forward[i] = newnode;
		}
	}
}
