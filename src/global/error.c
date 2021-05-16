/**
 * @file   error.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2017
 * @date   Sun Feb  5 15:37:43 2017
 *
 * @brief  RiNOO error handling
 *
 *
 */

__thread int rn_error;
__thread char rn_error_buf[4096];

const char *rn_error_string(void) {
    return rn_error_buf;
}