/**
 * @file   dns.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  DNS functions
 *
 *
 */

#include "rinoo/proto/dns/module.h"

void rn_dns_init(rn_sched_t *sched, rn_dns_t *dns, rn_dns_type_t type, const char *host)
{
	res_init();
	dns->socket = rn_udp_client(sched, (rn_addr_t *) &(_res.nsaddr_list[0]));
	dns->host = host;
	dns->answer = NULL;
	dns->authority = NULL;
	dns->additional = NULL;
	rn_buffer_init(&dns->buffer, dns->packet, sizeof(dns->packet));
	dns->type = type;
}

void rn_dns_destroy(rn_dns_t *dns)
{
	if (dns->answer != NULL) {
		free(dns->answer);
	}
	if (dns->authority != NULL) {
		free(dns->authority);
	}
	if (dns->additional != NULL) {
		free(dns->additional);
	}
	rn_socket_destroy(dns->socket);
}

int rn_dns_query(rn_dns_t *dns, rn_dns_type_t type, const char *host)
{
	char len;
	char *dot;
	unsigned short tmp;
	rn_dns_header_t header = { 0 };

	header.id = htons((unsigned short) ((unsigned long long) dns % USHRT_MAX));
	DNS_QUERY_SET_QR(header.flags, 0);
	DNS_QUERY_SET_OPCODE(header.flags, 0);
	DNS_QUERY_SET_AA(header.flags, 0);
	DNS_QUERY_SET_TC(header.flags, 0);
	DNS_QUERY_SET_RD(header.flags, 1);
	DNS_QUERY_SET_RA(header.flags, 0);
	DNS_QUERY_SET_Z(header.flags, 0);
	DNS_QUERY_SET_AD(header.flags, 0);
	DNS_QUERY_SET_CD(header.flags, 0);
	DNS_QUERY_SET_RCODE(header.flags, 0);
	header.flags = htons(header.flags);
	header.qdcount = htons(1);
	header.ancount = 0;
	header.nscount = 0;
	header.arcount = 0;
	rn_buffer_add(&dns->buffer, (char *) &header, sizeof(header));
	while (*host) {
		dot = strchrnul(host, '.');
		len = dot - host;
		rn_buffer_add(&dns->buffer, &len, 1);
		rn_buffer_add(&dns->buffer, host, (size_t) len);
		host += len;
		if (*host == '.') {
			host++;
		}
	}
	len = 0;
	rn_buffer_add(&dns->buffer, &len, 1);
	/* Query type */
	tmp = htons(type);
	rn_buffer_add(&dns->buffer, (char *) &tmp, sizeof(tmp));
	/* Query class */
	tmp = htons(1);
	rn_buffer_add(&dns->buffer, (char *) &tmp, sizeof(tmp));
	if (rn_socket_writeb(dns->socket, &dns->buffer) <= 0) {
		return -1;
	}
	return 0;
}

int rn_dns_get(rn_dns_t *dns, rn_dns_query_t *query, rn_addr_t *from)
{
	ssize_t count;
	rn_buffer_iterator_t iterator;

	rn_buffer_erase(&dns->buffer, 0);
	rn_buffer_init(&query->name.buffer, query->name.value, sizeof(query->name.value));
	count = rn_socket_recvfrom(dns->socket, rn_buffer_ptr(&dns->buffer), rn_buffer_msize(&dns->buffer), from);
	if (count <= 0) {
		return -1;
	}
	dns->buffer.size = count;
	rn_buffer_iterator_set(&iterator, &dns->buffer);
	if (rn_dns_header_get(&iterator, &dns->header) != 0) {
		return -1;
	}
	if (rn_dns_query_get(&iterator, query) != 0) {
		return -1;
	}
	return 0;
}

int rn_dns_reply_get(rn_dns_t *dns, uint32_t timeout)
{
	unsigned int i;
	rn_dns_query_t query;
	rn_buffer_iterator_t iterator;

	rn_buffer_erase(&dns->buffer, 0);
	rn_buffer_init(&query.name.buffer, query.name.value, sizeof(query.name.value));
	if (rn_socket_timeout(dns->socket, timeout) != 0) {
		return -1;
	}
	if (rn_socket_readb(dns->socket, &dns->buffer) <= 0) {
		return -1;
	}
	rn_buffer_iterator_set(&iterator, &dns->buffer);
	if (rn_dns_header_get(&iterator, &dns->header) != 0) {
		return -1;
	}
	if (dns->header.id != (unsigned short) ((unsigned long long) dns % USHRT_MAX)) {
		return -1;
	}
	if (rn_dns_query_get(&iterator, &query) != 0) {
		return -1;
	}
	if (query.type != dns->type) {
		return -1;
	}
	if (dns->header.ancount > 50) {
		return -1;
	}
	if (dns->header.nscount > 50) {
		return -1;
	}
	if (dns->header.arcount > 50) {
		return -1;
	}
	if (dns->header.ancount > 0) {
		dns->answer = malloc(sizeof(*dns->answer) * dns->header.ancount);
		if (dns->answer == NULL) {
			return -1;
		}
		for (i = 0; i < dns->header.ancount && !rn_buffer_iterator_end(&iterator); i++) {
			if (rn_dns_record_get(&iterator, &dns->answer[i]) != 0) {
				return -1;
			}
		}
	}
	if (dns->header.nscount > 0) {
		dns->authority = malloc(sizeof(*dns->authority) * dns->header.nscount);
		if (dns->authority == NULL) {
			return -1;
		}
		for (i = 0; i < dns->header.nscount && !rn_buffer_iterator_end(&iterator); i++) {
			if (rn_dns_record_get(&iterator, &dns->authority[i]) != 0) {
				return -1;
			}
		}
	}
	if (dns->header.arcount > 0) {
		dns->additional = malloc(sizeof(*dns->additional) * dns->header.arcount);
		if (dns->additional == NULL) {
			return -1;
		}
		for (i = 0; i < dns->header.arcount && !rn_buffer_iterator_end(&iterator); i++) {
			if (rn_dns_record_get(&iterator, &dns->additional[i]) != 0) {
				return -1;
			}
		}
	}
	return 0;
}

int rn_dns_addr_get(rn_sched_t *sched, const char *host, rn_addr_t *addr)
{
	unsigned int i;
	rn_dns_t dns;

	rn_dns_init(sched, &dns, DNS_TYPE_A, host);
	if (rn_dns_query(&dns, DNS_TYPE_A, host) != 0) {
		goto dns_error;
	}
	if (rn_dns_reply_get(&dns, 1000) != 0) {
		goto dns_error;
	}
	if (dns.header.ancount == 0) {
		goto dns_error;
	}
	i = 0;
	while (i < dns.header.ancount && dns.answer[i].type != DNS_TYPE_A) {
		i++;
	}
	if (i >= dns.header.ancount) {
		goto dns_error;
	}
	addr->v4.sin_family = AF_INET;
	addr->v4.sin_addr.s_addr = dns.answer[i].rdata.a.address;
	rn_dns_destroy(&dns);
	return 0;
dns_error:
	rn_dns_destroy(&dns);
	return -1;
}
