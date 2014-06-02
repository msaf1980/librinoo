/**
 * @file   dns.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Sun Oct 20 11:12:51 2013
 *
 * @brief  Header file for DNS structure
 *
 *
 */

#ifndef RINOO_PROTO_DNS_DNS_H_
#define RINOO_PROTO_DNS_DNS_H_

typedef enum s_rinoodns_type{
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
} t_rinoodns_type;

typedef struct s_rinoodns_name {
	char value[256];
	t_buffer buffer;
} t_rinoodns_name;

typedef struct s_rinoodns_header {
	unsigned short id;
	unsigned short flags;
	unsigned short qdcount;
	unsigned short ancount;
	unsigned short nscount;
	unsigned short arcount;
} t_rinoodns_header;

typedef struct s_rinoodns_query {
	t_rinoodns_name name;
	unsigned short type;
	unsigned short qclass;
} t_rinoodns_query;

/* RDATA */
/* Yes, there is some useless structs here, but it perfectly matches RFC. */

typedef struct s_rinoodns_rdata_a {
	unsigned long address;
} t_rinoodns_rdata_a;

typedef struct s_rinoodns_rdata_ns {
	t_rinoodns_name nsname;
} t_rinoodns_rdata_ns;

typedef struct s_rinoodns_rdata_cname {
	t_rinoodns_name cname;
} t_rinoodns_rdata_cname;

typedef struct s_rinoodns_rdata_soa {
	t_rinoodns_name mname;
	t_rinoodns_name rname;
	unsigned int serial;
	int refresh;
	int retry;
	int expire;
	unsigned int minimum;
} t_rinoodns_rdata_soa;

typedef struct s_rinoodns_rdata_ptr {
	t_rinoodns_name ptrname;
} t_rinoodns_rdata_ptr;

typedef struct s_rinoodns_rdata_hinfo {
	t_rinoodns_name cpu;
	t_rinoodns_name os;
} t_rinoodns_rdata_hinfo;

typedef struct s_rinoodns_rdata_mx {
	short preference;
	t_rinoodns_name exchange;
} t_rinoodns_rdata_mx;

typedef struct s_rinoodns_rdata_txt {
	t_rinoodns_name txtdata;
} t_rinoodns_rdata_txt;

typedef struct s_rinoodns_rdata_aaaa {
	char aaaadata[16];
} t_rinoodns_rdata_aaaa;

typedef union u_rinoodns_rdata {
	t_rinoodns_rdata_a a;
	t_rinoodns_rdata_ns ns;
	t_rinoodns_rdata_cname cname;
	t_rinoodns_rdata_soa soa;
	t_rinoodns_rdata_ptr ptr;
	t_rinoodns_rdata_hinfo hinfo;
	t_rinoodns_rdata_mx mx;
	t_rinoodns_rdata_txt txt;
	t_rinoodns_rdata_aaaa aaaa;
} t_rinoodns_rdata;

typedef struct s_rinoodns_record {
	t_rinoodns_name name;
	unsigned short type;
	unsigned short aclass;
	int ttl;
	unsigned short rdlength;
	t_rinoodns_rdata rdata;
} t_rinoodns_record;

typedef struct s_rinoodns {
	t_buffer buffer;
	char packet[512];
	const char *host;
	t_rinoodns_type type;
	t_rinoosocket *socket;
	t_rinoodns_header header;
	t_rinoodns_record *answer;
	t_rinoodns_record *authority;
	t_rinoodns_record *additional;
} t_rinoodns;

#define DNS_QUERY_SET_QR(flags, value)		do { flags = (flags & ~0x8000) | (value << 15); } while (0)
#define DNS_QUERY_SET_OPCODE(flags, value)	do { flags = (flags & ~0x7800) | (0x7800 & (value << 11)); } while (0)
#define DNS_QUERY_SET_AA(flags, value)		do { flags = (flags & ~0x0400) | (0x0400 & (value << 10)); } while (0)
#define DNS_QUERY_SET_TC(flags, value)		do { flags = (flags & ~0x0200) | (0x0200 & (value << 9)); } while (0)
#define DNS_QUERY_SET_RD(flags, value)		do { flags = (flags & ~0x0100) | (0x0100 & (value << 8)); } while (0)
#define DNS_QUERY_SET_RA(flags, value)		do { flags = (flags & ~0x0080) | (0x0080 & (value << 7)); } while (0)
#define DNS_QUERY_SET_Z(flags, value)		do { flags = (flags & ~0x0070) | (0x0070 & (value << 4)); } while (0)
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

void rinoo_dns_init(t_rinoosched *sched, t_rinoodns *dns, t_rinoodns_type type, const char *host);
void rinoo_dns_destroy(t_rinoodns *dns);
int rinoo_dns_ip_get(t_rinoosched *sched, const char *host, t_ip *ip);
int rinoo_dns_query(t_rinoodns *dns, t_rinoodns_type type, const char *host);
int rinoo_dns_header_get(t_buffer_iterator *iterator, t_rinoodns_header *header);
int rinoo_dns_name_get(t_buffer_iterator *iterator, t_buffer *name);
int rinoo_dns_rdata_get(t_buffer_iterator *iterator, size_t rdlength, t_rinoodns_type type, t_rinoodns_rdata *rdata);
int rinoo_dns_query_get(t_buffer_iterator *iterator, t_rinoodns_query *query);
int rinoo_dns_record_get(t_buffer_iterator *iterator, t_rinoodns_record *record);

#endif /* !RINOO_PROTO_DNS_DNS_H_ */
