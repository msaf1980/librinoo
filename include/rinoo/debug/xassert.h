/**
 * @file   xassert.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for assert definitions
 *
 *
 */

#ifndef RINOO_DEBUG_XASSERT_H_
#define RINOO_DEBUG_XASSERT_H_

#ifdef RINOO_DEBUG

#define XASSERT(expr, ret)	if (unlikely(!(expr))) { fprintf(stderr, "Error in function '%s' - %s:%d - %s\n", __FUNCTION__, __FILE__, __LINE__, #expr); return ret; }
#define XASSERTN(expr)		if (unlikely(!(expr))) { fprintf(stderr, "Error in function '%s' - %s:%d - %s\n", __FUNCTION__, __FILE__, __LINE__, #expr); return; }

#else

#define XASSERT(expr, ret)	if (unlikely(!(expr))) { return ret; }
#define XASSERTN(expr)		if (unlikely(!(expr))) { return; }

#endif /* !RINOO_DEBUG */

#endif /* !RINOO_DEBUG_XASSERT_H_ */
