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
	size_t size;
	t_rinoolist_node *head;
	t_rinoolist_node *tail;
	int (*compare)(t_rinoolist_node *node1, t_rinoolist_node *node2);
} t_rinoolist;

int rinoolist(t_rinoolist *list, int (*compare)(t_rinoolist_node *node1, t_rinoolist_node *node2));
void rinoolist_flush(t_rinoolist *list, void (*delete)(t_rinoolist_node *node1));
size_t rinoolist_size(t_rinoolist *list);
void rinoolist_put(t_rinoolist *list, t_rinoolist_node *node);
t_rinoolist_node *rinoolist_get(t_rinoolist *list, t_rinoolist_node *node);
int rinoolist_remove(t_rinoolist *list, t_rinoolist_node *node);
t_rinoolist_node *rinoolist_pop(t_rinoolist *list);
t_rinoolist_node *rinoolist_head(t_rinoolist *list);

#endif /* !RINOO_STRUCT_LIST_H_ */
