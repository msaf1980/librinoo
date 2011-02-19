/**
 * @file   list_destroy.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Apr 28 16:42:20 2010
 *
 * @brief  list_destroy unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

typedef struct	s_testnode
{
  int		value;
  char		tofree;
  char		freed;
}		t_testnode;

static t_testnode	tnodes[] =
  {
    { 42, 0, 0 },
    { 46, 0, 0 },
    { 43, 0, 0 },
    { 48, 1, 0 },
    { 45, 1, 0 },
    { 44, 1, 0 },
    { 47, 0, 0 },
    { 0, 0, 0 }
  };

int		cmp_func(void *node1, void *node2)
{
  if (node1 == node2)
    {
      return 0;
    }
  return 1;
}

void		free_func(void *node)
{
  int		i;
  int		value;

  value = PTR_TO_INT(node);
  for (i = 0; tnodes[i].value > 0; i++)
    {
      if (tnodes[i].value == value)
	{
	  XTEST(tnodes[i].tofree == 1);
	  tnodes[i].freed = 1;
	  return;
	}
    }
  XFAIL();
}

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int		main()
{
  int		i;
  t_list	*list;

  /* LIST_SORTED_HEAD test */
  list = list_create(LIST_SORTED_HEAD, cmp_func);
  XTEST(list != NULL);
  for (i = 0; tnodes[i].value > 0; i++)
    {
      XTEST(list_add(list,
		     INT_TO_PTR(tnodes[i].value),
		     (tnodes[i].tofree ? free_func : NULL)) != NULL);
    }
  list_destroy(list);
  for (i = 0; tnodes[i].value > 0; i++)
    {
      XTEST(tnodes[i].tofree == tnodes[i].freed);
      tnodes[i].freed = 0;
    }

  /* LIST_SORTED_TAIL test */
  list = list_create(LIST_SORTED_TAIL, cmp_func);
  XTEST(list != NULL);
  for (i = 0; tnodes[i].value > 0; i++)
    {
      XTEST(list_add(list,
		     INT_TO_PTR(tnodes[i].value),
		     (tnodes[i].tofree ? free_func : NULL)) != NULL);
    }
  list_destroy(list);
  for (i = 0; tnodes[i].value > 0; i++)
    {
      XTEST(tnodes[i].tofree == tnodes[i].freed);
      tnodes[i].freed = 0;
    }
  XPASS();
}
