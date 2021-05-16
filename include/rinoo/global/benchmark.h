#ifndef RINOO_BENCHMARKS_H_
#define RINOO_BENCHMARKS_H_

#include <time.h>

static inline unsigned long long clock_ns()
{
	struct timespec tm;
	clock_gettime(CLOCK_MONOTONIC, &tm);
	return 1000000000uLL * tm.tv_sec + tm.tv_nsec;
}

#endif /* RINOO_BENCHMARKS_H_ */
