/**
 * @file   murmurhash.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan  9 19:15:34 2011
 *
 * @brief  Murmurhash function prototypes
 *
 *
 */

#ifndef		RINOO_MURMURHASH_H_
# define	RINOO_MURMURHASH_H_

u32	murmurhash(const void *key, u32 len, u32 seed);
u64	murmurhash64(const void *key, u64 len, u64 seed);

#endif		/* !RINOO_MURMURHASH_H_ */
