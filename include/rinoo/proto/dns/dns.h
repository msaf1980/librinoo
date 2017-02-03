/**
 * @file   dns.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for DNS structure
 *
 *
 */

#ifndef RINOO_PROTO_DNS_DNS_H_
#define RINOO_PROTO_DNS_DNS_H_

typedef enum rn_dns_type_e {
	DNS_TYPE_A = 0x01,
	DNS_TYPE_NS = 0x02,
	/* DNS_TYPE_MD = 0x03, Obsolete */
	/* DNS_TYPE_MF = 0x04, Obsolete */
	DNS_TYPE_CNAME = 0x05,
	DNS_TYPE_SOA = 0x06,
	/* DNS_TYPE_MB = 0x07 Experimental */
	/* DNS_TYPE_MG = 0x08 Experimental */
	/* DNS_TYPE_MR = 0x09 Experimental */
	/* DNS_TYPE_NULL = 0x0a Experimental */
	/* DNS_TYPE_WKS = 0x0b Unused */
	DNS_TYPE_PTR = 0x0c,
	DNS_TYPE_HINFO = 0x0d,
	/* DNS_TYPE_MINFO = 0x0e, Experimental */
	DNS_TYPE_MX = 0x0f,
	DNS_TYPE_TXT = 0x10,
	DNS_TYPE_AAAA = 0x1c
} rn_dns_type_t;

typedef struct rn_dns_name_s {
	char value[256];
	rn_buffer_t buffer;
} rn_dns_name_t;

typedef struct rn_dns_header_s {
	unsigned short id;
	unsigned short flags;
	unsigned short qdcount;
	unsigned short ancount;
	unsigned short nscount;
	unsigned short arcount;
} rn_dns_header_t;

typedef struct rn_dns_query_s {
	rn_dns_name_t name;
	unsigned short type;
	unsigned short qclass;
} rn_dns_query_t;

/* RDATA */
/* Yes, there is some useless structs here, but it perfectly matches RFC. */

typedef struct rn_dns_rdata_a_s {
	unsigned long address;
} rn_dns_rdata_a_t;

typedef struct rn_dns_rdata_ns_s {
	rn_dns_name_t nsname;
} rn_dns_rdata_ns_t;

typedef struct rn_dns_rdata_cname_s {
	rn_dns_name_t cname;
} rn_dns_rdata_cname_t;

typedef struct rn_dns_rdata_soa_s {
	rn_dns_name_t mname;
	rn_dns_name_t rname;
	unsigned int serial;
	int refresh;
	int retry;
	int expire;
	unsigned int minimum;
} rn_dns_rdata_soa_t;

typedef struct rn_dns_rdata_ptr_s {
	rn_dns_name_t ptrname;
} rn_dns_rdata_ptr_t;

typedef struct rn_dns_rdata_hinfo_s {
	rn_dns_name_t cpu;
	rn_dns_name_t os;
} rn_dns_rdata_hinfo_t;

typedef struct rn_dns_rdata_mx_s {
	short preference;
	rn_dns_name_t exchange;
} rn_dns_rdata_mx_t;

typedef struct rn_dns_rdata_txt_s {
	rn_dns_name_t txtdata;
} rn_dns_rdata_txt_t;

typedef struct rn_dns_rdata_aaaa_s {
	char aaaadata[16];
} rn_dns_rdata_aaaa_t;

typedef union rn_dns_rdata_u {
	rn_dns_rdata_a_t a;
	rn_dns_rdata_ns_t ns;
	rn_dns_rdata_cname_t cname;
	rn_dns_rdata_soa_t soa;
	rn_dns_rdata_ptr_t ptr;
	rn_dns_rdata_hinfo_t hinfo;
	rn_dns_rdata_mx_t mx;
	rn_dns_rdata_txt_t txt;
	rn_dns_rdata_aaaa_t aaaa;
} rn_dns_rdata_t;

typedef struct rn_dns_record_s {
	rn_dns_name_t name;
	unsigned short type;
	unsigned short aclass;
	int ttl;
	unsigned short rdlength;
	rn_dns_rdata_t rdata;
} rn_dns_record_t;

typedef struct rn_dns_s {
	rn_buffer_t buffer;
	char packet[512];
	const char *host;
	rn_dns_type_t type;
	rn_socket_t *socket;
	rn_dns_header_t header;
	rn_dns_record_t *answer;
	rn_dns_record_t *authority;
	rn_dns_record_t *additional;
} rn_dns_t;

#define DNS_QUERY_SET_QR(flags, value)		do { flags = (flags & ~0x8000) | (value << 15); } while (0)
#define DNS_QUERY_SET_OPCODE(flags, value)	do { flags = (flags & ~0x7800) | (0x7800 & (value << 11)); } while (0)
#define DNS_QUERY_SET_AA(flags, value)		do { flags = (flags & ~0x0400) | (0x0400 & (value << 10)); } while (0)
#define DNS_QUERY_SET_TC(flags, value)		do { flags = (flags & ~0x0200) | (0x0200 & (value << 9)); } while (0)
#define DNS_QUERY_SET_RD(flags, value)		do { flags = (flags & ~0x0100) | (0x0100 & (value << 8)); } while (0)
#define DNS_QUERY_SET_RA(flags, value)		do { flags = (flags & ~0x0080) | (0x0080 & (value << 7)); } while (0)
#define DNS_QUERY_SET_Z(flags, value)		do { flags = (flags & ~0x0040) | (0x0040 & (value << 6)); } while (0)
#define DNS_QUERY_SET_AD(flags, value)		do { flags = (flags & ~0x0020) | (0x0020 & (value << 5)); } while (0)
#define DNS_QUERY_SET_CD(flags, value)		do { flags = (flags & ~0x0010) | (0x0010 & (value << 4)); } while (0)
#define DNS_QUERY_SET_RCODE(flags, value)	do { flags = (flags & ~0x000f) | (0x000f & (value)); } while (0)

#define DNS_QUERY_GET_QR(flags)			(flags & 0x8000)
#define DNS_QUERY_GET_OPCODE(flags)		(flags & 0x7800)
#define DNS_QUERY_GET_AA(flags)			(flags & 0x0400)
#define DNS_QUERY_GET_TC(flags)			(flags & 0x0200)
#define DNS_QUERY_GET_RD(flags)			(flags & 0x0100)
#define DNS_QUERY_GET_RA(flags)			(flags & 0x0080)
#define DNS_QUERY_GET_Z(flags)			(flags & 0x0070)
#define DNS_QUERY_GET_RCODE(flags)		(flags & 0x000f)

#define DNS_QUERY_NAME_IS_COMPRESSED(byte)	((byte & 0xc0) == 0xc0)
#define DNS_QUERY_NAME_GET_OFFSET(offset)	(offset & 0x3fff)

void rn_dns_init(rn_sched_t *sched, rn_dns_t *dns, rn_dns_type_t type, const char *host);
void rn_dns_destroy(rn_dns_t *dns);
int rn_dns_addr_get(rn_sched_t *sched, const char *host, rn_addr_t *addr);
int rn_dns_query(rn_dns_t *dns, rn_dns_type_t type, const char *host);
int rn_dns_header_get(rn_buffer_iterator_t *iterator, rn_dns_header_t *header);
int rn_dns_name_get(rn_buffer_iterator_t *iterator, rn_buffer_t *name);
int rn_dns_rdata_get(rn_buffer_iterator_t *iterator, size_t rdlength, rn_dns_type_t type, rn_dns_rdata_t *rdata);
int rn_dns_query_get(rn_buffer_iterator_t *iterator, rn_dns_query_t *query);
int rn_dns_record_get(rn_buffer_iterator_t *iterator, rn_dns_record_t *record);
int rn_dns_get(rn_dns_t *dns, rn_dns_query_t *query, rn_addr_t *from);
int rn_dns_reply_get(rn_dns_t *dns, uint32_t timeout);

#endif /* !RINOO_PROTO_DNS_DNS_H_ */
