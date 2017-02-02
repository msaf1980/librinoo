/**
 * @file   buffer_helper.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header for buffer helper
 *
 *
 */

#ifndef RINOO_MEMORY_BUFFER_HELPER_H_
#define RINOO_MEMORY_BUFFER_HELPER_H_

#define RN_BUFFER_HELPER_INISIZE	1024
#define RN_BUFFER_HELPER_MAXSIZE	(1024 * 1024 * 1024)

size_t rn_buffer_helper_growthsize(rn_buffer_t *buffer, size_t newsize);
void *rn_buffer_helper_malloc(rn_buffer_t *buffer, size_t size);
void *rn_buffer_helper_realloc(rn_buffer_t *buffer, size_t newsize);
int rn_buffer_helper_free(rn_buffer_t *buffer);

#endif /* !RINOO_MEMORY_BUFFER_HELPER_H_ */
