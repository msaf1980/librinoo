/**
 * @file   btree.h
 * @author Reginald Lips <reginald.l@gmail.com>
 * @date   Mon Jan 30 18:45:28 2012
 *
 * @brief  Header file for b+tree functions declarations.
 *
 *
 */

#ifndef		RINOO_STRUCT_BTREE_H_
# define	RINOO_STRUCT_BTREE_H_

# define	RINOO_BTREE_ORDER	5

typedef struct s_btreeleaf
{
	struct s_btreeleaf	*prev;
	struct s_btreeleaf	*next;
	t_listnode listnode;
} t_btreeleaf;

typedef struct s_btreenode
{
	size_t			size;
	t_btreeleaf		*head;
	struct s_btreenode	*children[RINOO_BTREE_ORDER + 1];
} t_btreenode;

typedef struct s_btree
{
	t_btreenode *root;
	t_listnode *head;
	size_t size;
	int (*cmp_func)(void *node1, void *node2);
} t_btree;

#endif		/* !RINOO_STRUCT_BTREE_H_ */
