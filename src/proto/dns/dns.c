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

int rinoo_dns_query(t_rinoodns *dns, t_rinoodns_query_type type)
{
	char len;
	char *point;
	const char *host;

	dns->header.id = htons((unsigned short) ((unsigned long long) dns % USHRT_MAX));
	DNS_QUERY_SET_QR(dns->header.flags, 0);
	DNS_QUERY_SET_OPCODE(dns->header.flags, 0);
	DNS_QUERY_SET_AA(dns->header.flags, 0);
	DNS_QUERY_SET_TC(dns->header.flags, 0);
	DNS_QUERY_SET_RD(dns->header.flags, 1);
	DNS_QUERY_SET_RA(dns->header.flags, 0);
	DNS_QUERY_SET_Z(dns->header.flags, 0);
	DNS_QUERY_SET_RCODE(dns->header.flags, 0);
	dns->header.flags = htons(dns->header.flags);
	dns->header.qdcount = htons(1);
	dns->header.ancount = 0;
	dns->header.nscount = 0;
	dns->header.arcount = 0;
	dns->query.type = htons(type);
	dns->query.qclass = htons(1);
	buffer_add(&dns->buffer, (char *) &dns->header, sizeof(dns->header));
	host = dns->host;
	while (*host) {
		point = strchrnul(host, '.');
		len = point - host;
		buffer_add(&dns->buffer, &len, 1);
		buffer_add(&dns->buffer, host, (size_t) len);
		host += len;
		if (*host == '.') {
			host++;
		}
	}
	len = 0;
	buffer_add(&dns->buffer, &len, 1);
	buffer_add(&dns->buffer, (char *) &dns->query.type, sizeof(dns->query.type));
	buffer_add(&dns->buffer, (char *) &dns->query.qclass, sizeof(dns->query.qclass));
	if (rinoo_socket_writeb(dns->socket, &dns->buffer) <= 0) {
		return -1;
	}
	return 0;
}

static int rinoo_dns_getshort(t_buffer *buffer, size_t *offset, unsigned short *value)
{
	if (*offset + sizeof(unsigned short) > buffer_size(buffer)) {
		return -1;
	}
	*value = ntohs(*(unsigned short *) (buffer_ptr(buffer) + *offset));
	*offset += sizeof(unsigned short);
	return 0;
}

static int rinoo_dns_getname(t_buffer *buffer, size_t *offset, t_buffer *name)
{
	char *label;
	size_t coffset;
	unsigned char size;
	unsigned short tmp;

	if (*offset + 1 >= buffer_size(buffer)) {
		return -1;
	}
	size = *(unsigned char *)(buffer_ptr(buffer) + *offset);
	while (size > 0) {
		if (DNS_QUERY_NAME_IS_COMPRESSED(size)) {
			if (rinoo_dns_getshort(buffer, offset, &tmp) != 0) {
				return -1;
			}
			coffset = DNS_QUERY_NAME_GET_OFFSET(tmp);
			if (rinoo_dns_getname(buffer, &coffset, name) != 0) {
				return -1;
			}
			*offset += 2;
		} else {
			(*offset)++;
			label = buffer_ptr(buffer) + *offset;
			if (*offset + size >= buffer_size(buffer)) {
				return -1;
			}
			if (buffer_size(name) > 0) {
				buffer_addstr(name, ".");
			}
			buffer_add(name, label, size);
			*offset += size;
		}
		if (*offset + 1 >= buffer_size(buffer)) {
			return -1;
		}
		size = *(unsigned char *)(buffer_ptr(buffer) + *offset);
	}
	(*offset)++;
	buffer_addnull(name);
	return 0;
}

int rinoo_dns_reply_parse(t_rinoodns *dns, uint32_t timeout)
{
	size_t offset;

	buffer_erase(&dns->buffer, 0);
	if (rinoo_socket_timeout(dns->socket, timeout) != 0) {
		return -1;
	}
	if (rinoo_socket_readb(dns->socket, &dns->buffer) <= 0) {
		return -1;
	}
	offset = 0;
	if (rinoo_dns_getshort(&dns->buffer, &offset, &dns->header.id) != 0) {
		return -1;
	}
	if (rinoo_dns_getshort(&dns->buffer, &offset, &dns->header.flags) != 0) {
		return -1;
	}
	if (rinoo_dns_getshort(&dns->buffer, &offset, &dns->header.qdcount) != 0) {
		return -1;
	}
	if (rinoo_dns_getshort(&dns->buffer, &offset, &dns->header.ancount) != 0) {
		return -1;
	}
	if (rinoo_dns_getshort(&dns->buffer, &offset, &dns->header.nscount) != 0) {
		return -1;
	}
	if (rinoo_dns_getshort(&dns->buffer, &offset, &dns->header.arcount) != 0) {
		return -1;
	}
	if (rinoo_dns_getname(&dns->buffer, &offset, &dns->query.buffer) != 0) {
		return -1;
	}
	if (rinoo_dns_getshort(&dns->buffer, &offset, &dns->query.type) != 0) {
		return -1;
	}
	if (rinoo_dns_getshort(&dns->buffer, &offset, &dns->query.qclass) != 0) {
		return -1;
	}
	if (rinoo_dns_getname(&dns->buffer, &offset, &dns->answer.buffer) != 0) {
		return -1;
	}
	if (rinoo_dns_getshort(&dns->buffer, &offset, &dns->answer.type) != 0) {
		return -1;
	}
	if (rinoo_dns_getshort(&dns->buffer, &offset, &dns->answer.aclass) != 0) {
		return -1;
	}
	if (rinoo_dns_getshort(&dns->buffer, &offset, &dns->answer.ttl) != 0) {
		return -1;
	}
	if (rinoo_dns_getshort(&dns->buffer, &offset, &dns->answer.rdlength) != 0) {
		return -1;
	}
	return 0;
}

int rinoo_dns(t_rinoosched *sched, const char *host, t_ip *ip)
{
	t_rinoodns dns;

	res_init();
	dns.host = host;
	dns.socket = rinoo_udp_client(sched, _res.nsaddr_list[0].sin_addr.s_addr, ntohs(_res.nsaddr_list[0].sin_port));
	buffer_set(&dns.buffer, dns.packet, sizeof(dns.packet));
	buffer_set(&dns.query.buffer, dns.query.name, sizeof(dns.query.name));
	buffer_set(&dns.answer.buffer, dns.answer.name, sizeof(dns.answer.name));
	rinoo_dns_query(&dns, DNS_QUERY_A);
	if (rinoo_dns_reply_parse(&dns, 1000) != 0) {
		rinoo_log("Error");
	}
	rinoo_log("Results: %d", dns.header.ancount);
	rinoo_socket_destroy(dns.socket);
	*ip = 0;
	return 0;
}
