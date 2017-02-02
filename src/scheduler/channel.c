/**
 * @file   channel.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2015
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief Channel functions
 *
 *
 */

#include "rinoo/scheduler/module.h"

/**
 * Create a new channel.
 *
 * @param sched Pointer to the scheduler to user.
 *
 * @return Pointer to the new channel, or NULL if an error occurs.
 */
rn_channel_t *rn_channel(rn_sched_t *sched)
{
	rn_channel_t *channel;

	channel = calloc(1, sizeof(*channel));
	if (channel == NULL) {
		return NULL;
	}
	channel->sched = sched;
	return channel;
}

/**
 * Destroy a channel.
 *
 * @param channel Channel to destroy.
 */
void rn_channel_destroy(rn_channel_t *channel)
{
	free(channel);
}

void *rn_channel_get(rn_channel_t *channel)
{
	void *result;
	rn_task_t *task;
	rn_sched_t *sched;

	sched = rn_scheduler_self();
	if (channel->sched != sched) {
		return NULL;
	}
	if (channel->buf == NULL) {
		channel->task = rn_task_self();
		rn_task_release(sched);
		if (channel->buf == NULL) {
			return NULL;
		}
	}
	result = channel->buf;
	task = channel->task;
	channel->buf = NULL;
	channel->size = 0;
	channel->task = NULL;
	rn_task_schedule(task, NULL);
	return result;
}

int rn_channel_put(rn_channel_t *channel, void *ptr)
{
	if (rn_channel_write(channel, ptr, 0) < 0) {
		return -1;
	}
	return 0;
}

/**
 * Read from a channel. This is blocking.
 *
 * @param channel Channel to read.
 * @param dest Pointer to memory where to store result.
 * @param size Size of memory read.
 *
 * @return number of bytes read on success, or -1 if an error occurs.
 */
int rn_channel_read(rn_channel_t *channel, void *dest, size_t size)
{
	rn_task_t *task;
	rn_sched_t *sched;

	sched = rn_scheduler_self();
	if (channel->sched != sched) {
		return -1;
	}
	if (channel->buf == NULL) {
		channel->task = rn_task_self();
		rn_task_release(sched);
		if (channel->buf == NULL) {
			return -1;
		}
	}
	if (size > channel->size) {
		size = channel->size;
	}
	memcpy(dest, channel->buf, size);
	channel->size -= size;
	if (channel->size > 0) {
		channel->buf += size;
	} else {
		task = channel->task;
		channel->buf = NULL;
		channel->size = 0;
		channel->task = NULL;
		rn_task_schedule(task, NULL);
	}
	return size;
}

/**
 * Write to a channel. This is blocking.
 *
 * @param channel Channel to read.
 * @param buf Buffer to write.
 * @param size Size of buf.
 *
 * @return Number of bytes written on success, or -1 if an error occurs.
 */
int rn_channel_write(rn_channel_t *channel, void *buf, size_t size)
{
	rn_task_t *task;
	rn_sched_t *sched;

	sched = rn_scheduler_self();
	if (channel->sched != sched) {
		return -1;
	}
	channel->buf = buf;
	channel->size = size;
	task = channel->task;
	if (task != NULL) {
		rn_task_schedule(task, NULL);
	}
	channel->task = rn_task_self();
	rn_task_release(sched);
	return size;
}
