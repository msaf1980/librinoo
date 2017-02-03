/**
 * @file   rn_rbtree.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief Header file for Red-Black tree implementation.
 *
 *
 */
/*-
 * Copyright 2002 Niels Provos <provos@citi.umich.edu>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this rn_list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this rn_list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef RINOO_STRUCT_RBTREE_H_
#define RINOO_STRUCT_RBTREE_H_

typedef enum rn_rbtree_color_e {
	RN_RBTREE_RED = 0,
	RN_RBTREE_BLACK
} rn_rbtree_color_t;

typedef struct rn_rbtree_node_s {
	rn_rbtree_color_t color;
	struct rn_rbtree_node_s *left;
	struct rn_rbtree_node_s *right;
	struct rn_rbtree_node_s *parent;
} rn_rbtree_node_t;

typedef struct rn_rbtree_s {
	uint64_t size;
	rn_rbtree_node_t *root;
	rn_rbtree_node_t *head;
	int (*compare)(rn_rbtree_node_t *node1, rn_rbtree_node_t *node2);
	void (*delete)(rn_rbtree_node_t *node);
} rn_rbtree_t;

int rn_rbtree(rn_rbtree_t *tree,
		int (*compare)(rn_rbtree_node_t *node1, rn_rbtree_node_t *node2),
		void (*delete)(rn_rbtree_node_t *node));
void rn_rbtree_flush(rn_rbtree_t *tree);
int rn_rbtree_put(rn_rbtree_t *tree, rn_rbtree_node_t *node);
void rn_rbtree_remove(rn_rbtree_t *tree, rn_rbtree_node_t *node);
rn_rbtree_node_t *rn_rbtree_head(rn_rbtree_t *tree);
rn_rbtree_node_t *rn_rbtree_next(rn_rbtree_node_t *node);
rn_rbtree_node_t *rn_rbtree_find(rn_rbtree_t *tree, rn_rbtree_node_t *node);

#endif /* !RINOO_STRUCT_RBTREE_H_ */
