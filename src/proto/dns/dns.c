/**
 * @file   dns.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Sun Oct 20 14:14:47 2013
 *
 * @brief  DNS functions
 *
 *
 */

#include "rinoo/rinoo.h"

int rinoo_dns_query(t_rinoodns *dns, t_rinoodns_type type, const char *host)
{
	char len;
	char *dot;
	unsigned short tmp;
	t_rinoodns_header header;

	header.id = htons((unsigned short) ((unsigned long long) dns % USHRT_MAX));
	DNS_QUERY_SET_QR(header.flags, 0);
	DNS_QUERY_SET_OPCODE(header.flags, 0);
	DNS_QUERY_SET_AA(header.flags, 0);
	DNS_QUERY_SET_TC(header.flags, 0);
	DNS_QUERY_SET_RD(header.flags, 1);
	DNS_QUERY_SET_RA(header.flags, 0);
	DNS_QUERY_SET_Z(header.flags, 0);
	DNS_QUERY_SET_RCODE(header.flags, 0);
	header.flags = htons(header.flags);
	header.qdcount = htons(1);
	header.ancount = 0;
	header.nscount = 0;
	header.arcount = 0;
	buffer_add(&dns->buffer, (char *) &header, sizeof(header));
	while (*host) {
		dot = strchrnul(host, '.');
		len = dot - host;
		buffer_add(&dns->buffer, &len, 1);
		buffer_add(&dns->buffer, host, (size_t) len);
		host += len;
		if (*host == '.') {
			host++;
		}
	}
	len = 0;
	buffer_add(&dns->buffer, &len, 1);
	/* Query type */
	tmp = htons(type);
	buffer_add(&dns->buffer, (char *) &tmp, sizeof(tmp));
	/* Query class */
	tmp = htons(1);
	buffer_add(&dns->buffer, (char *) &tmp, sizeof(tmp));
	if (rinoo_socket_writeb(dns->socket, &dns->buffer) <= 0) {
		return -1;
	}
	return 0;
}

int rinoo_dns_reply_get(t_rinoodns *dns, uint32_t timeout)
{
	unsigned int i;
	t_rinoodns_query query;
	t_buffer_iterator iterator;

	buffer_erase(&dns->buffer, 0);
	buffer_set(&query.name.buffer, query.name.value, sizeof(query.name.value));
	if (rinoo_socket_timeout(dns->socket, timeout) != 0) {
		return -1;
	}
	if (rinoo_socket_readb(dns->socket, &dns->buffer) <= 0) {
		return -1;
	}
	buffer_iterator_set(&iterator, &dns->buffer);
	if (rinoo_dns_getheader(&iterator, &dns->header) != 0) {
		return -1;
	}
	if (dns->header.id != (unsigned short) ((unsigned long long) dns % USHRT_MAX)) {
		return -1;
	}
	if (rinoo_dns_getquery(&iterator, &query) != 0) {
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
		for (i = 0; i < dns->header.ancount && !buffer_iterator_end(&iterator); i++) {
			if (rinoo_dns_getrecord(&iterator, &dns->answer[i]) != 0) {
				return -1;
			}
		}
	}
	if (dns->header.nscount > 0) {
		dns->authority = malloc(sizeof(*dns->authority) * dns->header.nscount);
		if (dns->authority == NULL) {
			return -1;
		}
		for (i = 0; i < dns->header.nscount && !buffer_iterator_end(&iterator); i++) {
			if (rinoo_dns_getrecord(&iterator, &dns->authority[i]) != 0) {
				return -1;
			}
		}
	}
	if (dns->header.arcount > 0) {
		dns->additional = malloc(sizeof(*dns->additional) * dns->header.arcount);
		if (dns->additional == NULL) {
			return -1;
		}
		for (i = 0; i < dns->header.arcount && !buffer_iterator_end(&iterator); i++) {
			if (rinoo_dns_getrecord(&iterator, &dns->additional[i]) != 0) {
				return -1;
			}
		}
	}
	return 0;
}

static void rinoo_dns_init(t_rinoosched *sched, t_rinoodns *dns, t_rinoodns_type type, const char *host)
{
	res_init();
	dns->socket = rinoo_udp_client(sched, _res.nsaddr_list[0].sin_addr.s_addr, ntohs(_res.nsaddr_list[0].sin_port));
	dns->host = host;
	dns->answer = NULL;
	dns->authority = NULL;
	dns->additional = NULL;
	buffer_set(&dns->buffer, dns->packet, sizeof(dns->packet));
	dns->type = type;
}

static void rinoo_dns_destroy(t_rinoodns *dns)
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
	rinoo_socket_destroy(dns->socket);
}

static void rinoo_dns_printrecord(t_rinoodns_record *record)
{
	switch (record->type) {
		case DNS_QUERY_A:
			rinoo_log("Record A: %s = %s", record->name.value, inet_ntoa(*(struct in_addr *) &record->rdata.a.address));
			break;
		case DNS_QUERY_NS:
			break;
		case DNS_QUERY_CNAME:
			break;
		case DNS_QUERY_SOA:
			rinoo_log("Record SOA: %s = %s %s %u %d %d %d %u",
					record->name.value,
					record->rdata.soa.mname.value,
					record->rdata.soa.rname.value,
					record->rdata.soa.serial,
					record->rdata.soa.refresh,
					record->rdata.soa.retry,
					record->rdata.soa.expire,
					record->rdata.soa.minimum);
			break;
		case DNS_QUERY_PTR:
			break;
		case DNS_QUERY_HINFO:
			break;
		case DNS_QUERY_MX:
			rinoo_log("Record MX: %s = %d %s", record->name.value, record->rdata.mx.preference, record->rdata.mx.exchange.value);
			break;
		case DNS_QUERY_TXT:
			rinoo_log("Record TXT: %s = %s", record->name.value, record->rdata.txt.txtdata.value);
			break;
	}
}

int rinoo_dns(t_rinoosched *sched, const char *host, t_ip *ip)
{
	unsigned int i;
	t_rinoodns dns;

	rinoo_dns_init(sched, &dns, DNS_QUERY_MX, host);
	if (rinoo_dns_query(&dns, DNS_QUERY_MX, host) != 0) {
		goto dns_error;
	}
	if (rinoo_dns_reply_get(&dns, 1000) != 0) {
		goto dns_error;
	}
	for (i = 0; i < dns.header.ancount; i++) {
		rinoo_dns_printrecord(&dns.answer[i]);
	}
	for (i = 0; i < dns.header.nscount; i++) {
		rinoo_dns_printrecord(&dns.authority[i]);
	}
	for (i = 0; i < dns.header.arcount; i++) {
		rinoo_dns_printrecord(&dns.additional[i]);
	}
	*ip = 0;
	rinoo_dns_destroy(&dns);
	return 0;
dns_error:
	rinoo_log("Error");
	rinoo_dns_destroy(&dns);
	return -1;
}
