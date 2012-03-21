/**
 * @file   list.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue Apr 27 10:16:26 2010
 *
 * @brief  Functions to manage linked list structures.
 *
 *
 */

#include "rinoo/rinoo.h"

/**
 * Creates a new list.
 *
 * @param type Type of the list to create.
 * @param cmp_func Pointer to a compare function.
 * @param free_func Pointer to a free function.
 *
 * @return A pointer to the new list, or NULL if an error occurs.
 */
t_rinoolist *rinoolist(t_rinoolist_type type, t_rinoolist_cmp cmp_func, t_rinoolist_free free_func)
{
	t_rinoolist *list;

	XASSERT(cmp_func != NULL, NULL);

	list = calloc(1, sizeof(*list));
	if (unlikely(list == NULL)) {
		return NULL;
	}
	list->type = type;
	list->cmp_func = cmp_func;
	list->free_func = free_func;
	return list;
}

/**
 * Destroys a list and its elements
 *
 * @param ptr Pointer to the list to destroy.
 */
void rinoolist_destroy(void *ptr)
{
	t_rinoolist *list;
	t_rinoolist_node *cur;
	t_rinoolist_node *tmp;

	XASSERTN(ptr != NULL);

	list = (t_rinoolist *) ptr;

	cur = list->head;
	while (cur != NULL) {
		tmp = cur->next;
		if (list->free_func != NULL) {
			list->free_func(cur->ptr);
		}
		free(cur);
		cur = tmp;
	}
	free(list);
}

/**
 * Inserts a list node between two others.
 *
 * @param list Pointer to the list.
 * @param new Pointer to the new element to insert.
 * @param prev Pointer to the previous element (NULL if head).
 * @param next Pointer to the next element (NULL if tail).
 */
void rinoolist_insertnode(t_rinoolist *list, t_rinoolist_node *new, t_rinoolist_node *prev, t_rinoolist_node *next)
{
	XASSERTN(list != NULL);
	XASSERTN(new != NULL);

	new->prev = prev;
	new->next = next;
	if (prev == NULL) {
		list->head = new;
	} else {
		prev->next = new;
	}
	if (next == NULL) {
		list->tail = new;
	} else {
		next->prev = new;
	}
	list->size++;
}

/**
 * Adds an already existing listnode to a list.
 *
 * @param list Pointer to the list.
 * @param new Pointer to the new listnode to add.
 *
 * @return 0 on success, -1 if an error occurs.
 */
int rinoolist_addnode(t_rinoolist *list, t_rinoolist_node *new)
{
	XASSERT(list != NULL, -1);
	XASSERT(new != NULL, -1);

	switch (list->type) {
	case RINOOLIST_SORTED_HEAD:
		new->next = list->head;
		new->prev = NULL;
		while (new->next != NULL &&
		       list->cmp_func(new->ptr, new->next->ptr) > 0) {
			new->prev = new->next;
			new->next = new->next->next;
		}
		break;
	case RINOOLIST_SORTED_TAIL:
		new->next = NULL;
		new->prev = list->tail;
		while (new->prev != NULL &&
		       list->cmp_func(new->ptr, new->prev->ptr) < 0) {
			new->next = new->prev;
			new->prev = new->prev->prev;
		}
		break;
	}
	rinoolist_insertnode(list, new, new->prev, new->next);
	return 0;
}

/**
 * Adds an element to a list.
 *
 * @param list Pointer to the list where to add the element.
 * @param ptr Pointer to the element to add.
 *
 * @return A pointer to the new list node, or NULL if an error occurs.
 */
t_rinoolist_node *rinoolist_add(t_rinoolist *list, void *ptr)
{
	t_rinoolist_node *new;

	XASSERT(list != NULL, NULL);

	new = calloc(1, sizeof(*new));
	if (unlikely(new == NULL)) {
		return NULL;
	}
	new->ptr = ptr;
	if (unlikely(rinoolist_addnode(list, new) != 0)) {
		free(new);
		return NULL;
	}
	return new;
}

/**
 * Deletes a list node from a list.
 *
 * @param list Pointer to the list to use.
 * @param node Pointer to the list node to remove.
 * @param needfree Boolean which indicates if the free_func has to be called.
 *
 * @return 0 on success, -1 if an error occurs.
 */
int rinoolist_removenode(t_rinoolist *list, t_rinoolist_node *node, u32 needfree)
{
	XASSERT(list != NULL, -1);
	XASSERT(node != NULL, -1);

	if (node->prev == NULL) {
		list->head = node->next;
	} else {
		node->prev->next = node->next;
	}
	if (node->next == NULL) {
		list->tail = node->prev;
	} else {
		node->next->prev = node->prev;
	}
	if (list->free_func != NULL && needfree == TRUE) {
		list->free_func(node->ptr);
	}
	free(node);
	list->size--;
	return 0;
}

/**
 * Deletes an element from a list.
 *
 * @param list Pointer to the list to use.
 * @param ptr Pointer to the element to remove.
 * @param needfree Boolean which indicates if the free_func has to be called.
 *
 * @return 0 on success, -1 if an error occurs.
 */
int rinoolist_remove(t_rinoolist *list, void *ptr, u32 needfree)
{
	t_rinoolist_node *cur;

	XASSERT(list != NULL, -1);

	cur = list->head;
	while (cur != NULL && list->cmp_func(cur->ptr, ptr) != 0) {
		cur = cur->next;
	}
	if (cur != NULL) {
		rinoolist_removenode(list, cur, needfree);
		return 0;
	}
	return -1;
}

/**
 * Finds an element in a list.
 *
 * @param list Pointer to the list to use.
 * @param ptr Pointer to an element to look for.
 *
 * @return Pointer to the element found or NULL if nothing is found.
 */
void *rinoolist_find(t_rinoolist *list, void *ptr)
{
	t_rinoolist_node *cur;

	XASSERT(list != NULL, NULL);

	cur = list->head;
	while (cur != NULL && list->cmp_func(cur->ptr, ptr) != 0) {
		cur = cur->next;
	}
	if (cur != NULL) {
		return cur->ptr;
	}
	return NULL;
}

/**
 * Gets head element and remove it from the list.
 *
 * @param list Pointer to the list to use.
 *
 * @return Pointer to the element or NULL if the list is empty.
 */
void *rinoolist_pophead(t_rinoolist *list)
{
	void *ptr;
	t_rinoolist_node *head;

	XASSERT(list != NULL, NULL);

	if (list->head == NULL) {
		return NULL;
	}
	head = list->head;
	list->head = head->next;
	if (list->head == NULL) {
		list->tail = NULL;
	} else {
		list->head->prev = NULL;
	}
	ptr = head->ptr;
	free(head);
	list->size--;
	return ptr;
}

/**
 * Gets a list node out of a list.
 *
 * @param list Pointer to the list to use.
 * @param node Pointer to the list node to pop.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int rinoolist_popnode(t_rinoolist *list, t_rinoolist_node *node)
{
	XASSERT(list != NULL, -1);
	XASSERT(node != NULL, -1);

	if (node->prev == NULL) {
		list->head = node->next;
	} else {
		node->prev->next = node->next;
	}
	if (node->next == NULL) {
		list->tail = node->prev;
	} else {
		node->next->prev = node->prev;
	}
	node->prev = NULL;
	node->next = NULL;
	list->size--;
	return 0;
}

/**
 * Gets head element of a list. Keeps the element in the list.
 *
 * @param list Pointer to the list to use.
 *
 * @return Pointer to the element found or NULL if the list is empty.
 */
void *rinoolist_gethead(t_rinoolist *list)
{
	XASSERT(list != NULL, NULL);

	if (list->head == NULL) {
		return NULL;
	}
	return list->head->ptr;
}

/**
 * Gets next element of a list. This element will be stored in
 * a list iterator to easily access to the next element in the
 * next call.
 *
 * @param list Pointer to the list to use.
 * @param iterator Pointer to a list iterator where to store the current element.
 *
 * @return A pointer to the current element or NULL if the end is reached.
 */
void *rinoolist_getnext(t_rinoolist *list, t_rinoolist_iter *iterator)
{
	XASSERT(list != NULL, NULL);
	XASSERT(iterator != NULL, NULL);

	if (unlikely(*iterator == NULL)) {
		*iterator = list->head;
	} else {
		*iterator = (*iterator)->next;
	}
	if (unlikely(*iterator == NULL)) {
		return NULL;
	}
	return (*iterator)->ptr;
}
