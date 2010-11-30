/**
 * @file   stdmem.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2010
 * @date   Mon Oct 26 13:39:56 2009
 *
 * @brief  Header file for stdmem function declarations
 *
 *
 */

#ifndef		RINOO_STDMEM_H_
# define	RINOO_STDMEM_H_

void		*xmalloc(size_t size);
void		*xcalloc(size_t nmemb, size_t size);
void		xfree(void *ptr);
void		*xrealloc(void *ptr, size_t size);

#endif		/* !RINOO_STDMEM_H_ */
