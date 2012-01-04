/**
 * @file   list.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue Apr 27 10:17:48 2010
 *
 * @brief  Header file for list functions declarations.
 *
 *
 */

#ifndef		RINOO_STRUCT_LIST_H_
# define	RINOO_STRUCT_LIST_H_

typedef enum s_listtype
  {
	  LIST_SORTED_HEAD = 0,
	  LIST_SORTED_TAIL
  } t_listtype;

typedef struct s_listnode
{
	void			*node;
	void			(*free_func)();
	struct s_listnode	*prev;
	struct s_listnode	*next;
} t_listnode;

typedef struct s_list
{
	u32		size;
	t_listtype	type;
	t_listnode	*head;
	t_listnode	*tail;
	int		(*cmp_func)(void *node1, void *node2);
} t_list;

typedef t_listnode	*t_listiterator;

t_list		*list_create(t_listtype type,
			     int (*cmp_func)(void *node1, void *node2));
void		list_destroy(void *ptr);
void		list_insertnode(t_list *list,
				t_listnode *new,
				t_listnode *prev,
				t_listnode *next);
int		list_addnode(t_list *list, t_listnode *new);
t_listnode	*list_add(t_list *list,
			  void *node,
			  void (*free_func)(void *node));
int		list_removenode(t_list *list, t_listnode *node, u32 needfree);
int		list_remove(t_list *list, void *node, u32 needfree);
void		*list_find(t_list *list, void *node);
void		*list_pophead(t_list *list);
int		list_popnode(t_list *list, t_listnode *node);
void		*list_gethead(t_list *list);
void		*list_getnext(t_list *list, t_listiterator *iterator);

#endif		/* !RINOO_STRUCT_LIST_H_ */
