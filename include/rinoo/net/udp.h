/**
 * @file   udp.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for UDP function declarations
 *
 *
 */

#ifndef RINOO_NET_UDP_H_
#define RINOO_NET_UDP_H_

rn_socket_t *rn_udp_client(rn_sched_t *sched, rn_ip_t *ip, uint16_t port);

#endif /* !RINOO_NET_UDP_H_ */
