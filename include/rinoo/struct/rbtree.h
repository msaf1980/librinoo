/**
 * @file   rbtree.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Fri Apr 13 10:43:43 2012
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
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
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

#ifndef		RINOO_STRUCT_RBTREE_H_
# define	RINOO_STRUCT_RBTREE_H_

typedef enum e_rinoorbtree_color
{
	RINOO_RBTREE_RED = 0,
	RINOO_RBTREE_BLACK
} t_rinoorbtree_color;

typedef struct s_rinoorbtree_node
{
	struct s_rinoorbtree_node	*left;
	struct s_rinoorbtree_node	*right;
	struct s_rinoorbtree_node	*parent;
	t_rinoorbtree_color		color;
} t_rinoorbtree_node;

typedef struct s_rinoorbtree
{
	u64			size;
	t_rinoorbtree_node	*root;
	t_rinoorbtree_node	*head;
	int			(*cmp_func)(t_rinoorbtree_node *node1,
					    t_rinoorbtree_node *node2);
	void			(*del_func)(t_rinoorbtree_node *node);
} t_rinoorbtree;

t_rinoorbtree *rinoorbtree(int (*cmp_func)(t_rinoorbtree_node *node1,
					   t_rinoorbtree_node *node2),
			   void (*del_func)(t_rinoorbtree_node *node));
void rinoorbtree_destroy(t_rinoorbtree *tree);
int rinoorbtree_put(t_rinoorbtree *tree, t_rinoorbtree_node *node);
void rinoorbtree_remove(t_rinoorbtree *tree, t_rinoorbtree_node *node);
t_rinoorbtree_node *rinoorbtree_head(t_rinoorbtree *tree);

#endif		/* !RINOO_STRUCT_RBTREE_H_ */
