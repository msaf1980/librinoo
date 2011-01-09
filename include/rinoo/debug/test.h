/**
 * @file   test.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Mon Oct 26 13:28:47 2009
 *
 * @brief  Header file for test macro declarations
 *
 *
 */

#ifndef		RINOO_TEST_H_
# define	RINOO_TEST_H_

# define	XTEST(expr)	if (!(expr)) { printf("Test failed in function '%s' - %s:%d - %s\n", __FUNCTION__, __FILE__, __LINE__, #expr); return (1); }
# define	XPASS()		do { printf("Passed\n"); return (0); } while (0)
# define	XFAIL()		do { printf("Failed\n"); return (1); } while (0)

#endif		/* !RINOO_TEST_H_ */
