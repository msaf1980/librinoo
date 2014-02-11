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

typedef enum s_rinoodns_query_type {
	DNS_QUERY_A = 0x01,
	DNS_QUERY_NS = 0x02,
	DNS_QUERY_MX = 0x0f,
	DNS_QUERY_TXT = 0x10
} t_rinoodns_query_type;

typedef struct s_rinoodns_header {
	unsigned short id;
	unsigned short flags;
	unsigned short qdcount;
	unsigned short ancount;
	unsigned short nscount;
	unsigned short arcount;
} t_rinoodns_header;

typedef struct s_rinoodns_query {
	t_buffer buffer;
	char name[256];
	unsigned short type;
	unsigned short qclass;
} t_rinoodns_query;

typedef struct s_rinoodns_answer {
	t_buffer buffer;
	char name[256];
	unsigned short type;
	unsigned short aclass;
	unsigned short ttl;
	unsigned short rdlength;
	char *data;
} t_rinoodns_answer;

typedef struct s_rinoodns {
	const char *host;
	t_buffer buffer;
	char packet[512];
	t_rinoosocket *socket;
	t_rinoodns_header header;
	t_rinoodns_query query;
	t_rinoodns_answer answer;
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

#define DNS_QUERY_NAME_IS_COMPRESSED(byte)	(byte & 0xC0)
#define DNS_QUERY_NAME_GET_OFFSET(offset)	(offset & 0x3FFF)

int rinoo_dns(t_rinoosched *sched, const char *host, t_ip *ip);
int rinoo_dns_query(t_rinoodns *dns, t_rinoodns_query_type type);

#endif /* !RINOO_PROTO_DNS_DNS_H_ */
