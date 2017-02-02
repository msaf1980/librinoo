/**
 * @file   test.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for test macro declarations
 *
 *
 */

#ifndef RINOO_DEBUG_TEST_H_
#define RINOO_DEBUG_TEST_H_

#define XTEST(expr)	if (!(expr)) { fprintf(stderr, "Test failed in function '%s' - %s:%d - %s\n", __FUNCTION__, __FILE__, __LINE__, #expr); XFAIL(); }
#define XPASS()		do { fprintf(stderr, "Passed\n"); return 0; } while (0)
#define XFAIL()		do { fprintf(stderr, "Failed\n"); exit(1); } while (0)

#endif /* !RINOO_DEBUG_TEST_H_ */
