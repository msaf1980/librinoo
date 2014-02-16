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

int rinoo_dns_query(t_rinoodns *dns)
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
	dns->query.type = htons(dns->type);
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

static int rinoo_dns_getname(t_buffer_iterator *iterator, t_buffer *name)
{
	char *label;
	unsigned char size;
	unsigned short tmp;
	t_buffer_iterator tmp_iter;

	while (!buffer_iterator_end(iterator)) {
		size = *(unsigned char *)(buffer_iterator_ptr(iterator));
		if (size == 0) {
			/* Move iterator position */
			buffer_iterator_getchar(iterator, NULL);
			buffer_addnull(name);
			return 0;
		} else if (DNS_QUERY_NAME_IS_COMPRESSED(size)) {
			if (buffer_iterator_gethushort(iterator, &tmp) != 0) {
				return -1;
			}
			buffer_iterator_set(&tmp_iter, iterator->buffer);
			if (buffer_iterator_position_set(&tmp_iter, DNS_QUERY_NAME_GET_OFFSET(tmp)) != 0) {
				return -1;
			}
			if (rinoo_dns_getname(&tmp_iter, name) != 0) {
				return -1;
			}
			/* Only end of domain can be compressed */
			return 0;
		} else {
			buffer_iterator_getchar(iterator, NULL);
			label = buffer_iterator_ptr(iterator);
			if (buffer_iterator_position_inc(iterator, size) != 0) {
				return -1;
			}
			if (buffer_size(name) > 0) {
				buffer_addstr(name, ".");
			}
			buffer_add(name, label, size);
		}
	}
	buffer_addnull(name);
	return 0;
}

static int rinoo_dns_getrdata(t_buffer_iterator *iterator, size_t rdlength, t_rinoodns_type type, t_rinoodns_rdata *rdata)
{
	int ip;
	size_t position;

	position = buffer_iterator_position_get(iterator);
	switch (type) {
		case DNS_QUERY_A:
			if (buffer_iterator_getint(iterator, &ip) != 0) {
				return -1;
			}
			rdata->a.address = *(t_ip *)(&ip);
			break;
		case DNS_QUERY_NS:
			if (rinoo_dns_getname(iterator, &rdata->ns.nsname.buffer) != 0) {
				return -1;
			}
			break;
		case DNS_QUERY_CNAME:
			if (rinoo_dns_getname(iterator, &rdata->cname.cname.buffer) != 0) {
				return -1;
			}
			break;
		case DNS_QUERY_SOA:
			if (rinoo_dns_getname(iterator, &rdata->soa.mname.buffer) != 0) {
				return -1;
			}
			if (rinoo_dns_getname(iterator, &rdata->soa.rname.buffer) != 0) {
				return -1;
			}
			if (buffer_iterator_gethuint(iterator, &rdata->soa.serial) != 0) {
				return -1;
			}
			if (buffer_iterator_gethint(iterator, &rdata->soa.refresh) != 0) {
				return -1;
			}
			if (buffer_iterator_gethint(iterator, &rdata->soa.retry) != 0) {
				return -1;
			}
			if (buffer_iterator_gethint(iterator, &rdata->soa.expire) != 0) {
				return -1;
			}
			if (buffer_iterator_gethuint(iterator, &rdata->soa.minimum) != 0) {
				return -1;
			}
			break;
		case DNS_QUERY_PTR:
			if (rinoo_dns_getname(iterator, &rdata->ptr.ptrname.buffer) != 0) {
				return -1;
			}
			break;
		case DNS_QUERY_HINFO:
			if (rinoo_dns_getname(iterator, &rdata->hinfo.cpu.buffer) != 0) {
				return -1;
			}
			if (rinoo_dns_getname(iterator, &rdata->hinfo.os.buffer) != 0) {
				return -1;
			}
			break;
		case DNS_QUERY_MX:
			if (buffer_iterator_gethshort(iterator, &rdata->mx.preference) != 0) {
				return -1;
			}
			if (rinoo_dns_getname(iterator, &rdata->mx.exchange.buffer) != 0) {
				return -1;
			}
			break;
		case DNS_QUERY_TXT:
			if (rinoo_dns_getname(iterator, &rdata->txt.txtdata.buffer) != 0) {
				return -1;
			}
			break;
		default:
			return -1;
	}
	if (buffer_iterator_position_get(iterator) - position != rdlength) {
		return -1;
	}
	return 0;
}

int rinoo_dns_reply_parse(t_rinoodns *dns, uint32_t timeout)
{
	t_buffer_iterator iterator;

	buffer_erase(&dns->buffer, 0);
	if (rinoo_socket_timeout(dns->socket, timeout) != 0) {
		return -1;
	}
	if (rinoo_socket_readb(dns->socket, &dns->buffer) <= 0) {
		return -1;
	}
	buffer_iterator_set(&iterator, &dns->buffer);
	if (buffer_iterator_gethushort(&iterator, &dns->header.id) != 0) {
		return -1;
	}
	if (buffer_iterator_gethushort(&iterator, &dns->header.flags) != 0) {
		return -1;
	}
	if (buffer_iterator_gethushort(&iterator, &dns->header.qdcount) != 0) {
		return -1;
	}
	if (buffer_iterator_gethushort(&iterator, &dns->header.ancount) != 0) {
		return -1;
	}
	if (buffer_iterator_gethushort(&iterator, &dns->header.nscount) != 0) {
		return -1;
	}
	if (buffer_iterator_gethushort(&iterator, &dns->header.arcount) != 0) {
		return -1;
	}
	if (rinoo_dns_getname(&iterator, &dns->query.name.buffer) != 0) {
		return -1;
	}
	if (buffer_iterator_gethushort(&iterator, &dns->query.type) != 0) {
		return -1;
	}
	if (buffer_iterator_gethushort(&iterator, &dns->query.qclass) != 0) {
		return -1;
	}
	if (rinoo_dns_getname(&iterator, &dns->answer.name.buffer) != 0) {
		return -1;
	}
	if (buffer_iterator_gethushort(&iterator, &dns->answer.type) != 0) {
		return -1;
	}
	if (buffer_iterator_gethushort(&iterator, &dns->answer.aclass) != 0) {
		return -1;
	}
	if (buffer_iterator_gethint(&iterator, &dns->answer.ttl) != 0) {
		return -1;
	}
	if (buffer_iterator_gethushort(&iterator, &dns->answer.rdlength) != 0) {
		return -1;
	}
	if (rinoo_dns_getrdata(&iterator, dns->answer.rdlength, dns->answer.type, &dns->answer.rdata) != 0) {
		return -1;
	}
	return 0;
}

static void rinoo_dns_init(t_rinoodns *dns, t_rinoodns_type type, const char *host, t_rinoosocket *socket)
{
	dns->host = host;
	dns->socket = socket;
	buffer_set(&dns->buffer, dns->packet, sizeof(dns->packet));
	buffer_set(&dns->query.name.buffer, dns->query.name.value, sizeof(dns->query.name.value));
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
	rinoo_dns_query(&dns);
	if (rinoo_dns_reply_parse(&dns, 1000) != 0) {
		rinoo_log("Error");
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
}
