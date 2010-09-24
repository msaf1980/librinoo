/**
 * @file   xtypes.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
 * @date   Mon Oct 26 14:09:29 2009
 *
 * @brief  Header file for type declarations
 *
 *
 */

#ifndef		RINOO_XTYPES_H_
# define	RINOO_XTYPES_H_

typedef unsigned int            u32;
typedef unsigned long long      u64;

# if		__WORDSIZE == 64
#  define	PTR_TO_U32(p)	((u32)((u64)(p) % UINT_MAX))
#  define	PTR_TO_INT(p)	((int)((u64)(p) % INT_MAX))
#  define	U32_TO_PTR(p)	((void*)(u64)(p))
#  define	INT_TO_PTR(p)	((void*)(u64)(p))
# else
#  define	PTR_TO_U32(p)	((u32)(p))
#  define	PTR_TO_INT(p)	((int)(p))
#  define	U32_TO_PTR(p)	((void*)(p))
#  define	INT_TO_PTR(p)	((void*)(p))
# endif		/* !__WORDSIZE == 64 */

# define	TRUE		1
# define	FALSE		0

#endif		/* !RINOO_XTYPES_H_ */
