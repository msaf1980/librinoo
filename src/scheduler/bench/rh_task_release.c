#include <getopt.h>

#include "rinoo/rinoo.h"

#include "rinoo/global/benchmark.h"

long long count = 10000000000;

rn_sched_t *sched;

void consumer(void *pargs)
{
	while(1) {
		count--;
		if (count < 1) {
			count = 0;			
			break;
		}
		rn_task_release(sched);
	}
	free(pargs);
}

static void usage(const char* procname) {
	printf("usage: %s -h [help] -c fibers -n loop\r\n", procname);
}

int main(int argc, char* argv[])
{
	int ch, x;
	int nfibers = 100000;
	long long iterations;
	long long start, duration;

	while ((ch = getopt(argc, argv, "hc:n:")) > 0) {
		switch (ch) {
		case 'h':
			usage(argv[0]);
			return 0;
		case 'c':
			nfibers = atoi(optarg);
            if (nfibers < 1) {
                nfibers = 1;
            }
			break;
		case 'n':
			count = atoll(optarg);
            if (count < 1) {
                count = 1;
            }
			break;
		default:
			break;
		}
	}

	iterations = count;

	sched = rn_scheduler();
	XTEST(sched != NULL);


	for (x = 0; x < nfibers; x++) {
		intptr_t *pargs = (intptr_t *)malloc(sizeof(intptr_t));
		*pargs = x;
        XTEST(rn_task_start(sched, consumer, pargs) == 0);
	}

	start = clock_ns();
	rn_scheduler_loop(sched);
	duration = clock_ns() - start;

	printf("rn_task_release (%lld yields, %d coroutines): %llu ns (%f/s)\n",
		iterations, nfibers, duration, 1000000000.0f * iterations / duration
	);

	rn_scheduler_destroy(sched);

	XPASS();
	return 0;
}