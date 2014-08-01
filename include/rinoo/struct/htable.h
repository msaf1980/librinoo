/**
 * @file   htable.h
 * @author reginaldl <reginald.@gmail.com> - Copyright 2013
 * @date   Tue Jul 29 20:53:47 2014
 *
 * @brief  Hash table structure
 *
 *
 */

#ifndef RINOO_STRUCT_HTABLE_H_
#define RINOO_STRUCT_HTABLE_H_

typedef struct s_rinoohtable_node {
	uint32_t hash;
	struct s_rinoohtable_node *prev;
	struct s_rinoohtable_node *next;
} t_rinoohtable_node;

typedef struct s_rinoohtable {
	size_t size;
	size_t table_size;
	t_rinoohtable_node **table;
	uint32_t (*hash)(t_rinoohtable_node *node);
	int (*compare)(t_rinoohtable_node *node1, t_rinoohtable_node *node2);
} t_rinoohtable;

int rinoohtable(t_rinoohtable *htable, size_t size, uint32_t (*hash)(t_rinoohtable_node *node), int (*compare)(t_rinoohtable_node *node1, t_rinoohtable_node *node2));
void rinoohtable_destroy(t_rinoohtable *htable);
void rinoohtable_flush(t_rinoohtable *htable, void (*delete)(t_rinoohtable_node *node1));
size_t rinoohtable_size(t_rinoohtable *htable);
void rinoohtable_put(t_rinoohtable *htable, t_rinoohtable_node *node);
t_rinoohtable_node *rinoohtable_get(t_rinoohtable *htable, t_rinoohtable_node *node);
int rinoohtable_remove(t_rinoohtable *htable, t_rinoohtable_node *node);

#endif /* !RINOO_STRUCT_HTABLE_H_ */
