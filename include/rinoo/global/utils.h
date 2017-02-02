/**
 * @file   utils.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for global utils
 *
 *
 */

#ifndef RINOO_UTILS_H_
#define RINOO_UTILS_H_

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))

#define RN_LOG_MAXLENGTH	2048

void rn_log(const char *format, ...);

#endif /* !RINOO_UTILS_H_ */
