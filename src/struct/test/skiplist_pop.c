/**
 * @file   skiplist_pop.c
 * @author Reginald Lips <reginald.l@gmail.com>
 * @date   Mon Mar 19 16:58:36 2012
 *
 * @brief  skiplist_pop unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

int cmp_func(void *unused(node1), void *unused(node2))
{
	int a = PTR_TO_INT(node1);
	int b = PTR_TO_INT(node2);
	return (a == b ? 0 : (a < b ? -1 : 1));
}

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int main()
{
	int i;
	int cur;
	int last;
	t_rinooskip *list;

	list = rinooskip(cmp_func, NULL);
	XTEST(list != NULL);
	for (i = 0; i < 1000; i++) {
		XTEST(rinooskip_add(list, INT_TO_PTR(random())) == 0);
	}
	for (i = 0; i < 1000; i++) {
		cur = PTR_TO_INT(rinooskip_pop(list));
		if (i != 0) {
			XTEST(cur <= last);
		}
		last = cur;
	}
	XTEST(rinooskip_pop(list) == NULL);
	rinooskip_destroy(list);
	XPASS();
}
