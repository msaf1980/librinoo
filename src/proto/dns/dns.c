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
	t_rinoodns_header header;
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
	if (rinoo_dns_getheader(&iterator, &header) != 0) {
		return -1;
	}
	if (header.id != (unsigned short) ((unsigned long long) dns % USHRT_MAX)) {
		return -1;
	}
	if (rinoo_dns_getquery(&iterator, &query) != 0) {
		return -1;
	}
	if (query.type != dns->type) {
		return -1;
	}
	if (rinoo_dns_getanswer(&iterator, dns->type, &dns->answer) != 0) {
		return -1;
	}
	return 0;
}

static void rinoo_dns_init(t_rinoodns *dns, t_rinoodns_type type, const char *host, t_rinoosocket *socket)
{
	dns->host = host;
	dns->socket = socket;
	buffer_set(&dns->buffer, dns->packet, sizeof(dns->packet));
	buffer_set(&dns->answer.name.buffer, dns->answer.name.value, sizeof(dns->answer.name.value));
	switch (type) {
		case DNS_QUERY_A:
			break;
		case DNS_QUERY_NS:
			buffer_set(&dns->answer.rdata.ns.nsname.buffer, dns->answer.rdata.ns.nsname.value, sizeof(dns->answer.rdata.ns.nsname.value));
			break;
		case DNS_QUERY_CNAME:
			buffer_set(&dns->answer.rdata.cname.cname.buffer, dns->answer.rdata.cname.cname.value, sizeof(dns->answer.rdata.cname.cname.value));
			break;
		case DNS_QUERY_SOA:
			buffer_set(&dns->answer.rdata.soa.mname.buffer, dns->answer.rdata.soa.mname.value, sizeof(dns->answer.rdata.soa.mname.value));
			buffer_set(&dns->answer.rdata.soa.rname.buffer, dns->answer.rdata.soa.rname.value, sizeof(dns->answer.rdata.soa.rname.value));
			break;
		case DNS_QUERY_PTR:
			buffer_set(&dns->answer.rdata.ptr.ptrname.buffer, dns->answer.rdata.ptr.ptrname.value, sizeof(dns->answer.rdata.ptr.ptrname));
			break;
		case DNS_QUERY_HINFO:
			buffer_set(&dns->answer.rdata.hinfo.cpu.buffer, dns->answer.rdata.hinfo.cpu.value, sizeof(dns->answer.rdata.hinfo.cpu.value));
			buffer_set(&dns->answer.rdata.hinfo.os.buffer, dns->answer.rdata.hinfo.os.value, sizeof(dns->answer.rdata.hinfo.os.value));
			break;
		case DNS_QUERY_MX:
			buffer_set(&dns->answer.rdata.mx.exchange.buffer, dns->answer.rdata.mx.exchange.value, sizeof(dns->answer.rdata.mx.exchange.value));
			break;
		case DNS_QUERY_TXT:
			buffer_set(&dns->answer.rdata.txt.txtdata.buffer, dns->answer.rdata.txt.txtdata.value, sizeof(dns->answer.rdata.txt.txtdata.value));
			break;
	}
	dns->type = type;
}

int rinoo_dns(t_rinoosched *sched, const char *host, t_ip *ip)
{
	t_rinoodns dns;
	t_rinoosocket *socket;

	res_init();
	socket = rinoo_udp_client(sched, _res.nsaddr_list[0].sin_addr.s_addr, ntohs(_res.nsaddr_list[0].sin_port));
	rinoo_dns_init(&dns, DNS_QUERY_A, host, socket);
	if (rinoo_dns_query(&dns, DNS_QUERY_A, host) != 0) {
		goto dns_error;
	}
	if (rinoo_dns_reply_get(&dns, 1000) != 0) {
		goto dns_error;
	}
	/*rinoo_log("Results: %s = %s %s %u %d %d %d %u",*/
			/*dns.answer.name.value,*/
			/*dns.answer.rdata.soa.mname.value,*/
			/*dns.answer.rdata.soa.rname.value,*/
			/*dns.answer.rdata.soa.serial,*/
			/*dns.answer.rdata.soa.refresh,*/
			/*dns.answer.rdata.soa.retry,*/
			/*dns.answer.rdata.soa.expire,*/
			/*dns.answer.rdata.soa.minimum);*/
	/*rinoo_log("Results: %s = %d %s", dns.answer.name.value, dns.answer.rdata.mx.preference, dns.answer.rdata.mx.exchange.value);*/
	/*rinoo_log("Results: %s = %s", dns.answer.name.value, dns.answer.rdata.txt.txtdata.value);*/
	rinoo_log("Results: %s = %s", dns.answer.name.value, inet_ntoa(*(struct in_addr *) &dns.answer.rdata.a.address));
	rinoo_socket_destroy(dns.socket);
	*ip = 0;
	return 0;
dns_error:
	rinoo_log("Error");
	rinoo_socket_destroy(socket);
	return -1;
}
