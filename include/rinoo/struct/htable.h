/**
 * @file   rn_htable.h
 * @author Reginald Lips <reginald.@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Hash table structure
 *
 *
 */

#ifndef RINOO_STRUCT_HTABLE_H_
#define RINOO_STRUCT_HTABLE_H_

typedef struct rn_htable_node_s {
	uint32_t hash;
	struct rn_htable_node_s *prev;
	struct rn_htable_node_s *next;
} rn_htable_node_t;

typedef struct rn_htable_s {
	size_t size;
	size_t table_size;
	rn_htable_node_t **table;
	uint32_t (*hash)(rn_htable_node_t *node);
	int (*compare)(rn_htable_node_t *node1, rn_htable_node_t *node2);
} rn_htable_t;

int rn_htable(rn_htable_t *rn_htable, size_t size, uint32_t (*hash)(rn_htable_node_t *node), int (*compare)(rn_htable_node_t *node1, rn_htable_node_t *node2));
void rn_htable_destroy(rn_htable_t *rn_htable);
void rn_htable_flush(rn_htable_t *rn_htable, void (*delete)(rn_htable_node_t *node1));
size_t rn_htable_size(rn_htable_t *rn_htable);
void rn_htable_put(rn_htable_t *rn_htable, rn_htable_node_t *node);
rn_htable_node_t *rn_htable_get(rn_htable_t *rn_htable, rn_htable_node_t *node);
int rn_htable_remove(rn_htable_t *rn_htable, rn_htable_node_t *node);

#endif /* !RINOO_STRUCT_HTABLE_H_ */
