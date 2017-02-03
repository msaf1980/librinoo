/**
 * @file   rn_rbtree.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief Red-Black tree implementation.
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

#include "rinoo/struct/module.h"

static inline void rn_rbtree_rotate_left(rn_rbtree_t *tree, rn_rbtree_node_t *node)
{
	rn_rbtree_node_t *current;

	current = node->right;
	node->right = current->left;
	if (node->right != NULL) {
		current->left->parent = node;
	}
	current->parent = node->parent;
	if (current->parent != NULL) {
		if (node == node->parent->left) {
			node->parent->left = current;
		} else {
			node->parent->right = current;
		}
	} else {
		tree->root = current;
	}
	current->left = node;
	node->parent = current;
}

static inline void rn_rbtree_rotate_right(rn_rbtree_t *tree, rn_rbtree_node_t *node)
{
	rn_rbtree_node_t *current;

	current = node->left;
	node->left = current->right;
	if (node->left != NULL) {
		current->right->parent = node;
	}
	current->parent = node->parent;
	if (current->parent != NULL) {
		if (node == node->parent->left) {
			node->parent->left = current;
		} else {
			node->parent->right = current;
		}
	} else {
		tree->root = current;
	}
	current->right = node;
	node->parent = current;
}

static void rn_rbtree_insert_color(rn_rbtree_t *tree, rn_rbtree_node_t *node)
{
	rn_rbtree_node_t *parent;
	rn_rbtree_node_t *gparent;
	rn_rbtree_node_t *current;

	parent = node->parent;
	while ((parent = node->parent) != NULL &&
	       parent->color == RN_RBTREE_RED &&
	       (gparent = parent->parent) != NULL) {

		if (parent == gparent->left) {
			current = gparent->right;
			if (current != NULL && current->color == RN_RBTREE_RED) {
				current->color = RN_RBTREE_BLACK;
				parent->color = RN_RBTREE_BLACK;
				gparent->color = RN_RBTREE_RED;
				node = gparent;
			} else {
				if (parent->right == node) {
					rn_rbtree_rotate_left(tree, parent);
					current = parent;
					parent = node;
					node = current;
				}
				parent->color = RN_RBTREE_BLACK;
				gparent->color = RN_RBTREE_RED;
				rn_rbtree_rotate_right(tree, gparent);
			}
		} else {
			current = gparent->left;
			if (current != NULL && current->color == RN_RBTREE_RED) {
				current->color = RN_RBTREE_BLACK;
				parent->color = RN_RBTREE_BLACK;
				gparent->color = RN_RBTREE_RED;
				node = gparent;
			} else {
				if (parent->left == node) {
					rn_rbtree_rotate_right(tree, parent);
					current = parent;
					parent = node;
					node = current;
				}
				parent->color = RN_RBTREE_BLACK;
				gparent->color = RN_RBTREE_RED;
				rn_rbtree_rotate_left(tree, gparent);
			}
		}
	}
	tree->root->color = RN_RBTREE_BLACK;
}

static void rn_rbtree_remove_color(rn_rbtree_t *tree, rn_rbtree_node_t *parent, rn_rbtree_node_t *node)
{
	rn_rbtree_node_t *current;

	while ((node == NULL || node->color == RN_RBTREE_BLACK) && node != tree->root && parent != NULL) {
		if (parent->left == node) {
			current = parent->right;
			if (current->color == RN_RBTREE_RED) {
				current->color = RN_RBTREE_BLACK;
				parent->color = RN_RBTREE_RED;
				rn_rbtree_rotate_left(tree, parent);
				current = parent->right;
			}
			if ((current->left == NULL || current->left->color == RN_RBTREE_BLACK) &&
			    (current->right == NULL || current->right->color == RN_RBTREE_BLACK)) {
				current->color = RN_RBTREE_RED;
				node = parent;
				parent = node->parent;
			} else {
				if (current->right == NULL || current->right->color == RN_RBTREE_BLACK) {
					if (current->left != NULL) {
						current->left->color = RN_RBTREE_BLACK;
					}
					current->color = RN_RBTREE_RED;
					rn_rbtree_rotate_right(tree, current);
					current = parent->right;
				}
				current->color = parent->color;
				parent->color = RN_RBTREE_BLACK;
				if (current->right != NULL) {
					current->right->color = RN_RBTREE_BLACK;
				}
				rn_rbtree_rotate_left(tree, parent);
				node = tree->root;
				break;
			}
		} else {
			current = parent->left;
			if (current->color == RN_RBTREE_RED) {
				current->color = RN_RBTREE_BLACK;
				parent->color = RN_RBTREE_RED;
				rn_rbtree_rotate_right(tree, parent);
				current = parent->left;
			}
			if ((current->left == NULL || current->left->color == RN_RBTREE_BLACK) &&
			    (current->right == NULL || current->right->color == RN_RBTREE_BLACK)) {
				current->color = RN_RBTREE_RED;
				node = parent;
				parent = node->parent;
			} else {
				if (current->left == NULL || current->left->color == RN_RBTREE_BLACK) {
					if (current->right != NULL) {
						current->right->color = RN_RBTREE_BLACK;
					}
					current->color = RN_RBTREE_RED;
					rn_rbtree_rotate_left(tree, current);
					current = parent->left;
				}
				current->color = parent->color;
				parent->color = RN_RBTREE_BLACK;
				if (current->left != NULL) {
					current->left->color = RN_RBTREE_BLACK;
				}
				rn_rbtree_rotate_right(tree, parent);
				node = tree->root;
				break;
			}
		}
	}

	if (node != NULL) {
		node->color = RN_RBTREE_BLACK;
	}
}

int rn_rbtree(rn_rbtree_t *tree, int (*compare)(rn_rbtree_node_t *node1, rn_rbtree_node_t *node2), void (*delete)(rn_rbtree_node_t *node))
{
	XASSERT(tree != NULL, -1);
	XASSERT(compare != NULL, -1);

	tree->root = NULL;
	tree->head = NULL;
	tree->compare = compare;
	tree->delete = delete;
	return 0;
}

void rn_rbtree_flush(rn_rbtree_t *tree)
{
	rn_rbtree_node_t *old;
	rn_rbtree_node_t *current;

	XASSERTN(tree != NULL);

	if (tree->root != NULL && tree->delete != NULL) {
		current = tree->head;
		while (current != NULL) {
			if (current->right != NULL) {
				current = current->right;
				while (current->left != NULL) {
					current = current->left;
				}
			} else {
				if (current->parent != NULL && (current == current->parent->left)) {
					old = current;
					current = current->parent;
					tree->delete(old);
				} else {
					while (current->parent != NULL && (current == current->parent->right)) {
						old = current;
						current = current->parent;
						tree->delete(old);
					}
					old = current;
					current = current->parent;
					tree->delete(old);
				}
			}
		}
	}
	tree->size = 0;
	tree->root = NULL;
	tree->head = NULL;
}

int rn_rbtree_put(rn_rbtree_t *tree, rn_rbtree_node_t *node)
{
	int cmp;
	int head;
	rn_rbtree_node_t *parent;
	rn_rbtree_node_t *current;

	XASSERT(tree != NULL, -1);
	XASSERT(node != NULL, -1);

	cmp = 0;
	head = 1;
	parent = NULL;
	current = tree->root;
	while (current != NULL) {
		parent = current;
		cmp = tree->compare(node, parent);
		if (cmp < 0) {
			current = current->left;
		} else if (likely(cmp > 0)) {
			current = current->right;
			head = 0;
		} else {
			return -1;
		}
	}
	node->left = NULL;
	node->right = NULL;
	node->parent = parent;
	node->color = RN_RBTREE_RED;
	if (parent != NULL) {
		if (cmp < 0) {
			parent->left = node;
		} else {
			parent->right = node;
			head = 0;
		}
	} else {
		tree->root = node;
	}
	if (head != 0) {
		tree->head = node;
	}

	rn_rbtree_insert_color(tree, node);
	tree->size++;

	return 0;
}

void rn_rbtree_remove(rn_rbtree_t *tree, rn_rbtree_node_t *node)
{
	rn_rbtree_node_t *old;
	rn_rbtree_node_t *child;
	rn_rbtree_node_t *parent;
	rn_rbtree_color_t color;

	XASSERTN(tree != NULL);
	XASSERTN(node != NULL);

	old = node;
	if (node->left == NULL) {
		child = node->right;
	} else if (node->right == NULL) {
		child = node->left;
	} else {
		node = node->right;
		while (node->left != NULL) {
			node = node->left;
		}
		child = node->right;
		parent = node->parent;
		color = node->color;
		if (child != NULL) {
			child->parent = parent;
		}
		if (parent != NULL) {
			if (parent->left == node) {
				parent->left = child;
			} else {
				parent->right = child;
			}
		} else {
			tree->root = child;
		}
		if (node->parent == old) {
			parent = node;
		}
		*node = *old;
		if (old->parent != NULL) {
			if (old->parent->left == old) {
				old->parent->left = node;
			} else {
				old->parent->right = node;
			}
		} else {
			tree->root = node;
		}
		old->left->parent = node;
		if (old->right != NULL) {
			old->right->parent = node;
		}
		goto color;
	}
	parent = node->parent;
	color = node->color;
	if (child != NULL) {
		child->parent = parent;
	}
	if (parent != NULL) {
		if (parent->left == node) {
			parent->left = child;
		} else {
			parent->right = child;
		}
	} else {
		tree->root = child;
	}

	if (node == tree->head) {
		if (parent != NULL && parent->left == NULL) {
			tree->head = parent;
		} else {
			tree->head = child;
		}
	}
color:
	if (color == RN_RBTREE_BLACK) {
		rn_rbtree_remove_color(tree, parent, child);
	}

	if (tree->delete != NULL) {
		tree->delete(old);
	}
	tree->size--;
}

rn_rbtree_node_t *rn_rbtree_head(rn_rbtree_t *tree)
{
	XASSERT(tree != NULL, NULL);

	return tree->head;
}

rn_rbtree_node_t *rn_rbtree_next(rn_rbtree_node_t *node)
{
	if (node->right != NULL) {
		node = node->right;
		while (node->left != NULL) {
			node = node->left;
		}
	} else {
		if (node->parent != NULL && (node == node->parent->left)) {
			node = node->parent;
		} else {
			while (node->parent != NULL && (node == node->parent->right)) {
				node = node->parent;
			}
			node = node->parent;
		}
	}

	return node;
}

rn_rbtree_node_t *rn_rbtree_find(rn_rbtree_t *tree, rn_rbtree_node_t *node)
{
	int cmp;
	rn_rbtree_node_t *tmp;

	XASSERT(tree != NULL, NULL);
	XASSERT(node != NULL, NULL);

	for (tmp = tree->root; tmp != NULL;) {
		cmp = tree->compare(node, tmp);
		if (cmp < 0) {
			tmp = tmp->left;
		} else if (cmp > 0) {
			tmp = tmp->right;
		} else {
			return tmp;
		}
	}

	return NULL;
}
