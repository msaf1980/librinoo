#ifndef RINOO_SCHEDULER_CHANNEL_H_
#define RINOO_SCHEDULER_CHANNEL_H_

typedef struct rn_channel_s {
	void *buf;
	size_t size;
	rn_task_t *task;
	rn_sched_t *sched;
} rn_channel_t;

rn_channel_t *rn_channel(rn_sched_t *sched);
void rn_channel_destroy(rn_channel_t *channel);
void *rn_channel_get(rn_channel_t *channel);
int rn_channel_put(rn_channel_t *channel, void *ptr);
int rn_channel_read(rn_channel_t *channel, void *dest, size_t size);
int rn_channel_write(rn_channel_t *channel, void *buf, size_t size);

#endif /* !RINOO_SCHEDULER_CHANNEL_H_ */
