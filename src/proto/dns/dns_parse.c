/**
 * @file   dns_parse.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2014
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  DNS parsing functions
 *
 *
 */

#include "rinoo/proto/dns/module.h"

int rn_dns_header_get(rn_buffer_iterator_t *iterator, rn_dns_header_t *header)
{
	if (rn_buffer_iterator_gethushort(iterator, &header->id) != 0) {
		return -1;
	}
	if (rn_buffer_iterator_gethushort(iterator, &header->flags) != 0) {
		return -1;
	}
	if (rn_buffer_iterator_gethushort(iterator, &header->qdcount) != 0) {
		return -1;
	}
	if (rn_buffer_iterator_gethushort(iterator, &header->ancount) != 0) {
		return -1;
	}
	if (rn_buffer_iterator_gethushort(iterator, &header->nscount) != 0) {
		return -1;
	}
	if (rn_buffer_iterator_gethushort(iterator, &header->arcount) != 0) {
		return -1;
	}
	return 0;
}

int rn_dns_name_get(rn_buffer_iterator_t *iterator, rn_buffer_t *name)
{
	char *label;
	unsigned char size;
	unsigned short tmp;
	rn_buffer_iterator_t tmp_iter;

	while (!rn_buffer_iterator_end(iterator)) {
		size = *(unsigned char *)(rn_buffer_iterator_ptr(iterator));
		if (size == 0) {
			/* Move iterator position */
			buffer_iterator_getchar(iterator, NULL);
			rn_buffer_addnull(name);
			return 0;
		} else if (DNS_QUERY_NAME_IS_COMPRESSED(size)) {
			if (rn_buffer_iterator_gethushort(iterator, &tmp) != 0) {
				return -1;
			}
			rn_buffer_iterator_set(&tmp_iter, iterator->buffer);
			if (rn_buffer_iterator_position_set(&tmp_iter, DNS_QUERY_NAME_GET_OFFSET(tmp)) != 0) {
				return -1;
			}
			if (rn_dns_name_get(&tmp_iter, name) != 0) {
				return -1;
			}
			/* Only end of domain can be compressed */
			return 0;
		} else {
			buffer_iterator_getchar(iterator, NULL);
			label = rn_buffer_iterator_ptr(iterator);
			if (rn_buffer_iterator_position_inc(iterator, size) != 0) {
				return -1;
			}
			if (rn_buffer_size(name) > 0) {
				rn_buffer_addstr(name, ".");
			}
			rn_buffer_add(name, label, size);
		}
	}
	rn_buffer_addnull(name);
	return 0;
}

int rn_dns_rdata_get(rn_buffer_iterator_t *iterator, size_t rdlength, rn_dns_type_t type, rn_dns_rdata_t *rdata)
{
	int ip;
	size_t position;

	position = rn_buffer_iterator_position_get(iterator);
	switch (type) {
		case DNS_TYPE_A:
			if (buffer_iterator_getint(iterator, &ip) != 0) {
				return -1;
			}
			rdata->a.address = ip;
			break;
		case DNS_TYPE_NS:
			if (rn_dns_name_get(iterator, &rdata->ns.nsname.buffer) != 0) {
				return -1;
			}
			break;
		case DNS_TYPE_CNAME:
			if (rn_dns_name_get(iterator, &rdata->cname.cname.buffer) != 0) {
				return -1;
			}
			break;
		case DNS_TYPE_SOA:
			if (rn_dns_name_get(iterator, &rdata->soa.mname.buffer) != 0) {
				return -1;
			}
			if (rn_dns_name_get(iterator, &rdata->soa.rname.buffer) != 0) {
				return -1;
			}
			if (rn_buffer_iterator_gethuint(iterator, &rdata->soa.serial) != 0) {
				return -1;
			}
			if (rn_buffer_iterator_gethint(iterator, &rdata->soa.refresh) != 0) {
				return -1;
			}
			if (rn_buffer_iterator_gethint(iterator, &rdata->soa.retry) != 0) {
				return -1;
			}
			if (rn_buffer_iterator_gethint(iterator, &rdata->soa.expire) != 0) {
				return -1;
			}
			if (rn_buffer_iterator_gethuint(iterator, &rdata->soa.minimum) != 0) {
				return -1;
			}
			break;
		case DNS_TYPE_PTR:
			if (rn_dns_name_get(iterator, &rdata->ptr.ptrname.buffer) != 0) {
				return -1;
			}
			break;
		case DNS_TYPE_HINFO:
			if (rn_dns_name_get(iterator, &rdata->hinfo.cpu.buffer) != 0) {
				return -1;
			}
			if (rn_dns_name_get(iterator, &rdata->hinfo.os.buffer) != 0) {
				return -1;
			}
			break;
		case DNS_TYPE_MX:
			if (rn_buffer_iterator_gethshort(iterator, &rdata->mx.preference) != 0) {
				return -1;
			}
			if (rn_dns_name_get(iterator, &rdata->mx.exchange.buffer) != 0) {
				return -1;
			}
			break;
		case DNS_TYPE_TXT:
			if (rn_dns_name_get(iterator, &rdata->txt.txtdata.buffer) != 0) {
				return -1;
			}
			break;
		case DNS_TYPE_AAAA:
			/* FIXME: Support of IPv6 */
			if (rn_buffer_iterator_position_inc(iterator, 16) != 0) {
				return -1;
			}
			break;
		default:
			return -1;
	}
	if (rn_buffer_iterator_position_get(iterator) - position != rdlength) {
		return -1;
	}
	return 0;
}

int rn_dns_query_get(rn_buffer_iterator_t *iterator, rn_dns_query_t *query)
{
	if (rn_dns_name_get(iterator, &query->name.buffer) != 0) {
		return -1;
	}
	if (rn_buffer_iterator_gethushort(iterator, &query->type) != 0) {
		return -1;
	}
	if (rn_buffer_iterator_gethushort(iterator, &query->qclass) != 0) {
		return -1;
	}
	return 0;
}

int rn_dns_record_get(rn_buffer_iterator_t *iterator, rn_dns_record_t *record)
{
	rn_buffer_set(&record->name.buffer, record->name.value, sizeof(record->name.value));
	if (rn_dns_name_get(iterator, &record->name.buffer) != 0) {
		return -1;
	}
	if (rn_buffer_iterator_gethushort(iterator, &record->type) != 0) {
		return -1;
	}
	switch (record->type) {
		case DNS_TYPE_A:
			break;
		case DNS_TYPE_NS:
			rn_buffer_set(&record->rdata.ns.nsname.buffer, record->rdata.ns.nsname.value, sizeof(record->rdata.ns.nsname.value));
			break;
		case DNS_TYPE_CNAME:
			rn_buffer_set(&record->rdata.cname.cname.buffer, record->rdata.cname.cname.value, sizeof(record->rdata.cname.cname.value));
			break;
		case DNS_TYPE_SOA:
			rn_buffer_set(&record->rdata.soa.mname.buffer, record->rdata.soa.mname.value, sizeof(record->rdata.soa.mname.value));
			rn_buffer_set(&record->rdata.soa.rname.buffer, record->rdata.soa.rname.value, sizeof(record->rdata.soa.rname.value));
			break;
		case DNS_TYPE_PTR:
			rn_buffer_set(&record->rdata.ptr.ptrname.buffer, record->rdata.ptr.ptrname.value, sizeof(record->rdata.ptr.ptrname));
			break;
		case DNS_TYPE_HINFO:
			rn_buffer_set(&record->rdata.hinfo.cpu.buffer, record->rdata.hinfo.cpu.value, sizeof(record->rdata.hinfo.cpu.value));
			rn_buffer_set(&record->rdata.hinfo.os.buffer, record->rdata.hinfo.os.value, sizeof(record->rdata.hinfo.os.value));
			break;
		case DNS_TYPE_MX:
			rn_buffer_set(&record->rdata.mx.exchange.buffer, record->rdata.mx.exchange.value, sizeof(record->rdata.mx.exchange.value));
			break;
		case DNS_TYPE_TXT:
			rn_buffer_set(&record->rdata.txt.txtdata.buffer, record->rdata.txt.txtdata.value, sizeof(record->rdata.txt.txtdata.value));
			break;
	}
	if (rn_buffer_iterator_gethushort(iterator, &record->aclass) != 0) {
		return -1;
	}
	if (rn_buffer_iterator_gethint(iterator, &record->ttl) != 0) {
		return -1;
	}
	if (rn_buffer_iterator_gethushort(iterator, &record->rdlength) != 0) {
		return -1;
	}
	if (rn_dns_rdata_get(iterator, record->rdlength, record->type, &record->rdata) != 0) {
		return -1;
	}
	return 0;
}
