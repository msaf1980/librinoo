/**
 * @file   list.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
 * @date   Tue Apr 27 10:17:48 2010
 *
 * @brief  Header file for list functions declarations.
 *
 *
 */

#ifndef		RINOO_LIST_H_
# define	RINOO_LIST_H_

typedef struct		s_listnode
{
  void			*node;
  void			(*free_func)();
  struct s_listnode	*prev;
  struct s_listnode	*next;
}			t_listnode;

typedef struct	s_list
{
  t_listnode	*head;
  t_listnode	*tail;
  u32		size;
  int		(*cmp_func)(void *node1, void *node2);
}		t_list;

typedef t_listnode	*t_listiterator;

t_list		*list_create(int (*cmp_func)(void *node1, void *node2));
void		list_destroy(void *ptr);
t_listnode	*list_add(t_list *list,
			  void *node,
			  void (*free_func)(void *node));
int		list_removenode(t_list *list, t_listnode *node, u32 needfree);
int		list_remove(t_list *list, void *node, u32 needfree);
void		*list_find(t_list *list, void *node);
void		*list_pophead(t_list *list);
void		*list_gethead(t_list *list);
void		*list_getnext(t_list *list, t_listiterator *iterator);

#endif		/* !RINOO_LIST_H_ */
