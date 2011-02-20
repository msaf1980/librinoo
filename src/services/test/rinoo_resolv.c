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

t_rinoosched	*sched;

void		result_cb(t_rinoodns *unused(rdns), t_rinoodns_event event, t_ip ip)
{
  char		tab[16];

  switch (event)
    {
    case RINOODNS_EVENT_OK:
      inet_ntop(AF_INET, &ip, tab, 16);
      printf("%u %s\n", ip, tab);
      if (ip != 2642114907)
	{
	  XFAIL();
	}
      break;
    case RINOODNS_EVENT_ERROR:
      XFAIL();
      break;
    }
  rinoo_sched_stop(sched);
}

void		result_cb2(t_rinoodns *unused(rdns),
			   t_rinoodns_event event,
			   t_ip ip)
{
  char		tab[16];

  if (event == RINOODNS_EVENT_ERROR)
    {
      XFAIL();
    }
  inet_ntop(AF_INET, &ip, tab, 16);
  printf("%u %s\n", ip, tab);
  rinoo_sched_stop(sched);
}

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int		main()
{
  sched = rinoo_sched();
  XTEST(sched != NULL);
  XTEST(rinoo_resolv(sched, "puffy.fr", RINOODNS_TYPE_A, 0, result_cb) != NULL);
  rinoo_sched_loop(sched);
  XTEST(rinoo_resolv(sched, "www.google.fr", RINOODNS_TYPE_A, 0, result_cb2) != NULL);
  rinoo_sched_loop(sched);
  XTEST(rinoo_resolv(sched, "www.optionbit.com", RINOODNS_TYPE_A, 0, result_cb2) != NULL);
  rinoo_sched_loop(sched);
  rinoo_sched_destroy(sched);
  XPASS();
}
