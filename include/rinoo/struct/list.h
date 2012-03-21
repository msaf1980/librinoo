/**
 * @file   list.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue Apr 27 10:17:48 2010
 *
 * @brief  Header file for list function declarations.
 *
 *
 */

#ifndef		RINOO_STRUCT_LIST_H_
# define	RINOO_STRUCT_LIST_H_

typedef int	(*t_rinoolist_cmp)(void *ptr1, void *ptr2);
typedef void	(*t_rinoolist_free)(void *ptr);

typedef enum s_rinoolist_type
{
	RINOOLIST_SORTED_HEAD = 0,
	RINOOLIST_SORTED_TAIL
} t_rinoolist_type;

typedef struct s_rinoolist_node
{
	void			*ptr;
	struct s_rinoolist_node	*prev;
	struct s_rinoolist_node	*next;
} t_rinoolist_node;

typedef struct s_rinoolist
{
	u32			size;
	t_rinoolist_type	type;
	t_rinoolist_node	*head;
	t_rinoolist_node	*tail;
	t_rinoolist_cmp		cmp_func;
	t_rinoolist_free	free_func;
} t_rinoolist;

typedef t_rinoolist_node	*t_rinoolist_iter;

t_rinoolist *rinoolist(t_rinoolist_type type, t_rinoolist_cmp cmp_func, t_rinoolist_free free_func);
void rinoolist_destroy(void *ptr);
void rinoolist_insertnode(t_rinoolist *list, t_rinoolist_node *new, t_rinoolist_node *prev, t_rinoolist_node *next);
int rinoolist_addnode(t_rinoolist *list, t_rinoolist_node *new);
t_rinoolist_node *rinoolist_add(t_rinoolist *list, void *ptr);
int rinoolist_removenode(t_rinoolist *list, t_rinoolist_node *node, u32 needfree);
int rinoolist_remove(t_rinoolist *list, void *ptr, u32 needfree);
void *rinoolist_find(t_rinoolist *list, void *ptr);
void *rinoolist_pophead(t_rinoolist *list);
int rinoolist_popnode(t_rinoolist *list, t_rinoolist_node *node);
void *rinoolist_gethead(t_rinoolist *list);
void *rinoolist_getnext(t_rinoolist *list, t_rinoolist_iter *iterator);

#endif		/* !RINOO_STRUCT_LIST_H_ */
