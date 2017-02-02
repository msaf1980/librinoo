/**
 * @file   rn_list.h
 * @author Reginald Lips <reginald.@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  LIST structure
 *
 *
 */

#ifndef RINOO_STRUCT_LIST_H_
#define RINOO_STRUCT_LIST_H_

typedef struct rn_list_node_s {
	struct rn_list_node_s *prev;
	struct rn_list_node_s *next;
} rn_list_node_t;

typedef struct rn_list_s {
	size_t size;
	rn_list_node_t *head;
	rn_list_node_t *tail;
	int (*compare)(rn_list_node_t *node1, rn_list_node_t *node2);
} rn_list_t;

int rn_list(rn_list_t *rn_list, int (*compare)(rn_list_node_t *node1, rn_list_node_t *node2));
void rn_list_flush(rn_list_t *rn_list, void (*delete)(rn_list_node_t *node1));
size_t rn_list_size(rn_list_t *rn_list);
void rn_list_put(rn_list_t *rn_list, rn_list_node_t *node);
rn_list_node_t *rn_list_get(rn_list_t *rn_list, rn_list_node_t *node);
int rn_list_remove(rn_list_t *rn_list, rn_list_node_t *node);
rn_list_node_t *rn_list_pop(rn_list_t *rn_list);
rn_list_node_t *rn_list_head(rn_list_t *rn_list);

#endif /* !RINOO_STRUCT_LIST_H_ */
