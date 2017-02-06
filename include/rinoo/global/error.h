/**
 * @file   error.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2017
 * @date   Sun Feb  5 15:39:19 2017
 *
 * @brief  RiNOO error handling
 *
 *
 */

extern __thread int rn_error;

#define rn_error_set(err)	do { rn_error = (err); } while (0)

