/**
 * @file   skiplist.c
 * @author Reginald Lips <reginald.l@gmail.com>
 * @date   Mon Mar 19 16:58:36 2012
 *
 * @brief  skiplist unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

int cmp_func(void *unused(node1), void *unused(node2))
{
	return 1;
}

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int main()
{
	t_rinooskip *list;

	list = rinooskip(cmp_func, NULL);
	XTEST(list != NULL);
	rinooskip_destroy(list);
	XPASS();
}
