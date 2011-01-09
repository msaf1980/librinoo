/**
 * @file   poller.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Mon Dec 28 02:37:33 2009
 *
 * @brief  This file describes a poller API which is an abstraction of select/epoll/poll...
 *
 *
 */

#include	"rinoo/rinoo.h"

t_pollerapi	pollers[NB_POLLERS] =
  {
    { xselect_init, xselect_destroy, xselect_insert, xselect_addmode, xselect_delmode, xselect_remove, xselect_poll },
    { xepoll_init, xepoll_destroy, xepoll_insert, xepoll_addmode, xepoll_delmode, xepoll_remove, xepoll_poll }
  };
