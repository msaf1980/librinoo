/**
 * @file   stcp.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue Jun  5 10:45:19 2012
 *
 * @brief  Header file for Secure TCP socket function declaractions
 *
 *
 */

#ifndef		RINOO_NET_STCP_H_
# define	RINOO_NET_STCP_H_

typedef struct s_rinoossl
{
	X509 *x509;
	EVP_PKEY *pkey;
} t_rinoossl;

#endif		/* !RINOO_NET_STCP_H_ */
