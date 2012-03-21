/**
 * @file   task.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Thu Oct 13 22:15:13 2011
 *
 * @brief  Scheduler task functions
 *
 *
 */

#include "rinoo/rinoo.h"

#ifdef RINOO_DEBUG
# include <valgrind/valgrind.h>
#endif

static int rinoo_task_cmp(void *ptr1, void *ptr2)
{
	t_rinootask *task1;
	t_rinootask *task2;

	task1 = ptr1;
	task2 = ptr2;
	if (task1 == task2) {
		return 0;
	}
	if (timercmp(&task1->tv, &task2->tv, <=)) {
		return -1;
	}
	return 1;
}

/**
 * Task driver initialization.
 * It sets a task driver in a scheduler.
 *
 * @param sched Pointer to the scheduler to set
 *
 * @return 0 on success, -1 if an error occurs
 */
int rinoo_task_driver_init(t_rinoosched *sched)
{
	t_rinootask_driver *driver;

	XASSERT(sched != NULL, -1);
	XASSERT(sched->task_driver == NULL, -1);

	driver = calloc(1, sizeof(*driver));
	if (driver == NULL) {
		return -1;
	}
	driver->cur_task = &driver->main_task;
	driver->task_list = rinooskip(rinoo_task_cmp, NULL);
	if (driver->task_list == NULL) {
		rinoo_task_driver_destroy(sched);
		return -1;
	}
	sched->task_driver = driver;
	return 0;
}

/**
 * Destroy internal task driver from a scheduler.
 *
 * @param sched Pointer to the scheduler to use
 */
void rinoo_task_driver_destroy(t_rinoosched *sched)
{
	XASSERTN(sched != NULL);

	if (sched->task_driver != NULL) {
		if (sched->task_driver->task_list != NULL) {
			rinooskip_destroy(sched->task_driver->task_list);
		}
		free(sched->task_driver);
		sched->task_driver = NULL;
	}
}

u32 rinoo_task_driver_run(t_rinoosched *sched)
{
	t_rinootask *cur;
	struct timeval tv;

	XASSERT(sched != NULL, 1000);
	XASSERT(sched->task_driver != NULL, 1000);

	while ((cur = rinooskip_head(sched->task_driver->task_list)) != NULL &&
	       timercmp(&cur->tv, &sched->clock, <=)) {
		cur = rinooskip_pop(sched->task_driver->task_list);
		rinoo_task_run(cur);
	}
	if (cur != NULL) {
		timersub(&cur->tv, &sched->clock, &tv);
		return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	}
	return 1000;
}

/**
 * Internal routine called for each context.
 * When calling makecontext, parameters passed must be integers.
 * This function transforms its two parameters into a t_rinootask pointer
 * and calls the task function.
 *
 * @param p1 First part of t_rinootask pointer
 * @param p2 Second part of t_rinootask pointer
 */
static void rinoo_task_process(int p1, int p2)
{
	t_rinootask *task;
	t_rinootask_arg targ;

	targ.args[0] = p1;
	targ.args[1] = p2;
	task = targ.ptr;
	XASSERTN(task != NULL);
	XASSERTN(task->func != NULL);
	task->func(task->sched, task->arg);
	/* That means we can destroy the task */
	task->func = NULL;
}

/**
 * Create a new task.
 *
 * @param sched Pointer to a scheduler to use
 * @param task_func Routine to call for that task
 * @param arg Argument to be passed to task_func
 *
 * @return A pointer to the new task on success, or NULL if an error occurs
 */
t_rinootask *rinoo_task(t_rinoosched *sched,
			t_rinootask_func task_func,
			void *arg)
{
	t_rinootask *new;
	t_rinootask_arg targ;

	XASSERT(sched != NULL, NULL);
	XASSERT(sched->task_driver != NULL, NULL);
	XASSERT(task_func != NULL, NULL);

	new = calloc(1, sizeof(*new));
	if (new == NULL) {
		return NULL;
	}
	if (getcontext(&new->context) != 0) {
		free(new);
		return NULL;
	}
	new->sched = sched;
	new->func = task_func;
	new->arg = arg;
	new->context.uc_stack.ss_sp = new->stack;
	new->context.uc_stack.ss_size = sizeof(new->stack);
	new->context.uc_link = &sched->task_driver->cur_task->context;

#ifdef RINOO_DEBUG
	/* This code avoids valgrind to mix stack switches */
	new->valgrind_id = VALGRIND_STACK_REGISTER(new->stack, new->stack + sizeof(new->stack));
#endif /* !RINOO_DEBUG */

	targ.ptr = new;
	makecontext(&new->context, (void (*)()) rinoo_task_process, 2, targ.args[0], targ.args[1]);
	return new;
}

int rinoo_task_schedule(t_rinootask *task, struct timeval *tv)
{
	XASSERT(task != NULL, -1);
	XASSERT(task->sched != NULL, -1);
	XASSERT(task->sched->task_driver != NULL, -1);

	if (tv != NULL) {
		task->tv = *tv;
	}
	if (rinooskip_add(task->sched->task_driver->task_list, task) != 0) {
		return -1;
	}
	return 0;
}

/**
 * Run or resume a task.
 * This function switches to the task stack by calling swapcontext(3).
 *
 * @param task Pointer to the task to run or resume
 *
 * @return 0 on success, -1 if an error occurs
 */
int rinoo_task_run(t_rinootask *task)
{
	int ret;
	t_rinootask *old;

	XASSERT(task != NULL, -1);

	old = task->sched->task_driver->cur_task;
	task->sched->task_driver->cur_task = task;
	ret = swapcontext(&old->context, &task->context);
	task->sched->task_driver->cur_task = old;
	if (task->func == NULL) {
		rinoo_task_destroy(task);
	}
	return ret;
}

/**
 * Release execution of a task currently running on a scheduler.
 *
 * @param sched Pointer to the scheduler to use
 *
 * @return 0 on success or errno if an error occurs
 */
int rinoo_task_release(t_rinoosched *sched)
{
	XASSERT(sched != NULL, -1);
	XASSERT(sched->task_driver != NULL, -1);

	errno = 0;
	if (swapcontext(&sched->task_driver->cur_task->context,
			&sched->task_driver->main_task.context) != 0) {
		return -1;
	}
	return errno;
}

/**
 * Destroy a task.
 *
 * @param task Pointer to the task to destroy
 */
void rinoo_task_destroy(t_rinootask *task)
{
	XASSERTN(task != NULL);

#ifdef RINOO_DEBUG
	VALGRIND_STACK_DEREGISTER(task->valgrind_id);
#endif /* !RINOO_DEBUG */

	free(task);
}
