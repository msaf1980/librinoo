/**
 * @file   tcp.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for TCP function declarations
 *
 *
 */

#ifndef RINOO_NET_TCP_H_
#define RINOO_NET_TCP_H_

#define RN_TCP_BACKLOG	128

rn_socket_t *rn_tcp_client(rn_sched_t *sched, rn_addr_t *dst, uint32_t timeout);
rn_socket_t *rn_tcp_server(rn_sched_t *sched, rn_addr_t *dst);
rn_socket_t *rn_tcp_accept(rn_socket_t *socket, rn_addr_t *from);

#endif /* !RINOO_NET_TCP_H_ */
