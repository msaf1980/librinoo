/**
 * @file   skiplist.h
 * @author Reginald Lips <reginald.l@gmail.com>
 * @date   Fri Mar 16 15:38:59 2012
 *
 * @brief  Header file for skip list function declarations.
 *
 *
 */

#ifndef		RINOO_STRUCT_SKIPLIST_H_
# define	RINOO_STRUCT_SKIPLIST_H_

# define	RINOO_SKIPLIST_MAXLEVEL		15

typedef struct s_rinooskip_node
{
	int			level;
	void			*ptr;
	struct s_rinooskip_node	**forward;
} t_rinooskip_node;

typedef struct s_rinooskip
{
	u64			size;
	int			level;
	t_rinooskip_node	head;
	t_rinoolist_cmp		cmp_func;
	t_rinoolist_free	free_func;
} t_rinooskip;

t_rinooskip *rinooskip(t_rinoolist_cmp cmp_func, t_rinoolist_free free_func);
void rinooskip_destroy(t_rinooskip *list);
int rinooskip_add(t_rinooskip *list, void *ptr);
void *rinooskip_pop(t_rinooskip *list);
void *rinooskip_head(t_rinooskip *list);

#endif		/* !RINOO_STRUCT_SKIPLIST_H_ */
