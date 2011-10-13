/**
 * @file   macros.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Sun Dec 27 23:32:50 2009
 *
 * @brief  Header file for global macros
 *
 *
 */

#ifndef		RINOO_MACROS_H_
# define	RINOO_MACROS_H_

# if defined(__GNUC__) && \
  (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96))
#  define	likely(x)	__builtin_expect((x), 1)
#  define	unlikely(x)	__builtin_expect((x), 0)
# else
#  define	likely(x)	(x)
#  define	unlikely(x)	(x)
# endif

# if defined(__GNUC__)
#  define	unused(x)	x __attribute__((unused))
# else
#  define	unused(x)	x
# endif

#endif		/* !RINOO_MACROS_H_ */
