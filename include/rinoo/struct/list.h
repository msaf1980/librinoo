/**
 * @file   list.h
 * @author reginaldl <reginald.@gmail.com> - Copyright 2013
 * @date   Sun Mar 24 23:18:12 2013
 *
 * @brief  LIST structure
 *
 *
 */

#ifndef RINOO_STRUCT_LIST_H_
#define RINOO_STRUCT_LIST_H_

typedef struct s_rinoolist_node {
	struct s_rinoolist_node *prev;
	struct s_rinoolist_node *next;
} t_rinoolist_node;

typedef struct s_rinoolist {
	size_t count;
	t_rinoolist_node *head;
	t_rinoolist_node *tail;
} t_rinoolist;

void rinoolist_add(t_rinoolist *list, t_rinoolist_node *node);
int rinoolist_remove(t_rinoolist *list, t_rinoolist_node *node);
t_rinoolist_node *rinoolist_pop(t_rinoolist *list);

#endif /* !RINOO_STRUCT_LIST_H_ */
