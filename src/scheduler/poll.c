/**
 * @file   poll.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue Mar 20 15:35:13 2012
 *
 * @brief  This file describes an API working as an abstraction of select/poll/epoll
 *
 *
 */


#include	"rinoo/rinoo.h"

t_rinoopoll	pollers[RINOO_NB_POLLS] =
  {
    { NULL, rinoo_epoll_init, rinoo_epoll_destroy, rinoo_epoll_insert, rinoo_epoll_addmode, rinoo_epoll_remove, rinoo_epoll_poll }
  };
