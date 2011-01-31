/**
 * @file   rinoo_resolv.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan 30 18:21:55 2011
 *
 * @brief  rinoo_resolv unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

t_sched		*sched;
int		passed = 0;

void		result_cb(t_dnsevent event, t_ip ip)
{
  char		tab[16];

  switch (event)
    {
    case EVENT_DNS_OK:
      inet_ntop(AF_INET, &ip, tab, 16);
      printf("%u %s\n", ip, tab);
      if (ip == 2642114907)
	{
	  passed = 1;
	}
      break;
    case EVENT_DNS_ERROR:
      break;
    }
  sched_stop(sched);
}

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int		main()
{
  sched = sched_create();
  XTEST(sched != NULL);
  XTEST(rinoo_resolv(sched, "puffy.fr", TYPE_DNS_A, 0, result_cb) == 0);
  sched_loop(sched);
  sched_destroy(sched);
  if (passed == 0)
    {
      XFAIL();
    }
  XPASS();
}
