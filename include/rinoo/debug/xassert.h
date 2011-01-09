/**
 * @file   xassert.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Mon Oct 26 13:15:55 2009
 *
 * @brief  Header file for assert definitions
 *
 *
 */

#ifndef		RINOO_XASSERT_H_
# define	RINOO_XASSERT_H_

# ifdef		RINOO_DEBUG

# define	XASSERT(expr, ret)		if (!(expr)) { fprintf(stderr, "Error in function '%s' - %s:%d - %s\n", __FUNCTION__, __FILE__, __LINE__, #expr); return (ret); }
# define	XASSERTN(expr)			if (!(expr)) { fprintf(stderr, "Error in function '%s' - %s:%d - %s\n", __FUNCTION__, __FILE__, __LINE__, #expr); return; }
# define	XASSERTSTR(expr, ret, str)	if (!(expr)) { fprintf(stderr, "%s / Error in function '%s' - %s:%d - %s\n", str, __FUNCTION__, __FILE__, __LINE__, #expr); return (ret); }
# define	XASSERTNSTR(expr, str)		if (!(expr)) { fprintf(stderr, "%s / Error in function '%s' - %s:%d - %s\n", str, __FUNCTION__, __FILE__, __LINE__, #expr); return; }
# define	XDASSERT(expr, ret)		if (!(expr)) { fprintf(stderr, "Error in function '%s' - %s:%d - %s\n", __FUNCTION__, __FILE__, __LINE__, #expr); return (ret); }
# define	XDASSERTN(expr)			if (!(expr)) { fprintf(stderr, "Error in function '%s' - %s:%d - %s\n", __FUNCTION__, __FILE__, __LINE__, #expr); return; }
# define	XDASSERTSTR(expr, ret, str)	if (!(expr)) { fprintf(stderr, "%s / Error in function '%s' - %s:%d - %s\n", str, __FUNCTION__, __FILE__, __LINE__, #expr); return (ret); }
# define	XDASSERTNSTR(expr, str)		if (!(expr)) { fprintf(stderr, "%s / Error in function '%s' - %s:%d - %s\n", str, __FUNCTION__, __FILE__, __LINE__, #expr); return; }

# else

# define	XASSERT(expr, ret)		if (!(expr)) { return (ret); }
# define	XASSERTN(expr)			if (!(expr)) { return; }
# define	XASSERTSTR(expr, ret, str)	XASSERT(expr, ret)
# define	XASSERTNSTR(expr, str)		XASSERTN(expr)
# define	XDASSERT(expr, ret)
# define	XDASSERTN(expr)
# define	XDASSERTSTR(expr, ret, str)
# define	XDASSERTNSTR(expr, str)


# endif		/* !RINOO_DEBUG */

#endif		/* !RINOO_XASSERT_H_ */
