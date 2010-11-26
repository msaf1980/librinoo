/**
 * @file   utils.h
 * @author Reginald <reginald.l@gmail.com> - Copyright 2010
 * @date   Wed Oct  6 21:46:14 2010
 *
 * @brief  Header file for global utils
 *
 *
 */

#ifndef		RINOO_UTILS_H_
# define	RINOO_UTILS_H_

# define	MIN(a, b) (a > b ? b : a)

int		timeraddms(struct timeval *in, u32 ms, struct timeval *out);
void		rinoo_log(const char *format, ...);

#endif		/* !RINOO_UTILS_H_ */
