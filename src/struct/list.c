/**
 * @file   list.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
 * @date   Tue Apr 27 10:16:26 2010
 *
 * @brief  Functions to manage linked list structures.
 *
 *
 */

#include	"rinoo/rinoo.h"

/**
 * Creates a new list.
 *
 *
 * @return Pointer to the new list, or NULL if an error occurs.
 */
t_list		*list_create(int (*cmp_func)(void *node1, void *node2))
{
  t_list	*list;

  XDASSERT(cmp_func != NULL, NULL);

  list = xcalloc(1, sizeof(*list));
  XASSERT(list != NULL, NULL);

  list->cmp_func = cmp_func;
  return (list);
}

/**
 * Destroys a list and its elements
 *
 * @param ptr Pointer to the list to destroy.
 */
void		list_destroy(void *ptr)
{
  t_list	*list;
  t_listnode	*cur;
  t_listnode	*tmp;

  XDASSERTN(ptr != NULL);

  list = (t_list *) ptr;

  cur = list->head;
  while (cur != NULL)
    {
      tmp = cur->next;
      if (cur->free_func != NULL)
	cur->free_func(cur->node);
      xfree(cur);
      cur = tmp;
    }
  xfree(list);
}

/**
 * Adds an element to a list.
 *
 * @param list Pointer to the list where to add the element.
 * @param node Pointer to the element to add.
 * @param free_func Pointer to a function which can free the element.
 *
 * @return A pointer to the new list node, or NULL if an error occurs.
 */
t_listnode	*list_add(t_list *list,
			  void *node,
			  void (*free_func)(void *node))
{
  t_listnode	*new;

  XDASSERT(list != NULL, NULL);

  new = xcalloc(1, sizeof(*new));
  XASSERT(new != NULL, NULL);
  new->node = node;
  new->free_func = free_func;
  new->next = list->head;
  new->prev = NULL;
  while (new->next != NULL &&
	 list->cmp_func(new->next->node, node) > 0)
    {
      new->prev = new->next;
      new->next = new->next->next;
    }
  if (new->next == NULL)
    {
      list->tail = new;
    }
  else
    {
      new->next->prev = new;
    }
  if (new->prev == NULL)
    {
      list->head = new;
    }
  else
    {
      new->prev->next = new;
    }
  list->size++;
  return (new);
}

/**
 * Deletes a list node from a list.
 *
 * @param list Pointer to the list to use.
 * @param node Pointer to the list node to remove.
 * @param needfree Boolean which indicates if the free_func has to be called.
 *
 * @return 1 on success, 0 if an error occurs.
 */
int		list_removenode(t_list *list, t_listnode *node, u32 needfree)
{
  XDASSERT(list != NULL, FALSE);
  XDASSERT(node != NULL, FALSE);

  if (node->prev == NULL)
    {
      list->head = node->next;
    }
  else
    {
      node->prev->next = node->next;
    }
  if (node->next == NULL)
    {
      list->tail = node->prev;
    }
  else
    {
      node->next->prev = node->prev;
    }
  if (node->free_func != NULL && needfree == TRUE)
    {
      node->free_func(node->node);
    }
  xfree(node);
  list->size--;
  return (TRUE);
}

/**
 * Deletes an element from a list.
 *
 * @param list Pointer to the list to use.
 * @param node Pointer to the element to remove.
 * @param needfree Boolean which indicates if the free_func has to be called.
 *
 * @return 1 on success, 0 if an error occurs.
 */
int		list_remove(t_list *list, void *node, u32 needfree)
{
  t_listnode	*cur;

  XDASSERT(list != NULL, FALSE);

  cur = list->head;
  while (cur != NULL &&
	 list->cmp_func(cur->node, node) != 0)
    {
      cur = cur->next;
    }
  if (cur != NULL)
    {
      list_removenode(list, cur, needfree);
      return (TRUE);
    }
  return (FALSE);
}

/**
 * Finds an element in a list.
 *
 * @param list Pointer to the list to use.
 * @param node Pointer to an element to look for.
 *
 * @return Pointer to the element found or NULL if nothing is found.
 */
void		*list_find(t_list *list, void *node)
{
  t_listnode	*cur;

  XDASSERT(list != NULL, NULL);

  cur = list->head;
  while (cur != NULL &&
	 list->cmp_func(cur->node, node) != 0)
    cur = cur->next;
  if (cur != NULL)
    return (cur->node);
  return (NULL);
}

/**
 * Gets head element and remove it from the list.
 *
 * @param list Pointer to the list to use.
 *
 * @return Pointer to the element or NULL if the list is empty.
 */
void		*list_pophead(t_list *list)
{
  t_listnode	*head;
  void		*node;

  XDASSERT(list != NULL, NULL);

  if (list->head == NULL)
    return (NULL);
  head = list->head;
  list->head = head->next;
  if (list->head == NULL)
    {
      list->tail = NULL;
    }
  else
    {
      list->head->prev = NULL;
    }
  node = head->node;
  xfree(head);
  list->size--;
  return (node);
}

/**
 * Gets head element of a list. Keeps the element in the list.
 *
 * @param list Pointer to the list to use.
 *
 * @return Pointer to the element found or NULL if the list is empty.
 */
void		*list_gethead(t_list *list)
{
  XDASSERT(list != NULL, NULL);

  if (list->head == NULL)
    return (NULL);
  return (list->head->node);
}
