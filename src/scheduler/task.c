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

static int rinoo_task_cmp(t_rinoorbtree_node *node1, t_rinoorbtree_node *node2)
{
	t_rinootask *task1 = container_of(node1, t_rinootask, proc_node);
	t_rinootask *task2 = container_of(node2, t_rinootask, proc_node);

	if (task1 == task2) {
		return 0;
	}
	if (timercmp(&task1->tv, &task2->tv, <)) {
		return -1;
	}
	return 1;
}

/**
 * Task driver initialization.
 * It sets the task driver in a scheduler.
 *
 * @param sched Pointer to the scheduler to set
 *
 * @return 0 on success, -1 if an error occurs
 */
int rinoo_task_driver_init(t_rinoosched *sched)
{
	XASSERT(sched != NULL, -1);

	if (rinoorbtree(&sched->driver.proc_tree, rinoo_task_cmp, NULL) != 0) {
		return -1;
	}
	sched->driver.current = &sched->driver.main;
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

	rinoorbtree_flush(&sched->driver.proc_tree);
}

u32 rinoo_task_driver_run(t_rinoosched *sched)
{
	t_rinootask *task;
	struct timeval tv;
	t_rinoorbtree_node *head;

	XASSERT(sched != NULL, 1000);

	while ((head = rinoorbtree_head(&sched->driver.proc_tree)) != NULL) {
		task = container_of(head, t_rinootask, proc_node);
		if (timercmp(&task->tv, &sched->clock, <=)) {
			rinoo_task_unschedule(task);
			rinoo_task_resume(task);
		} else {
			timersub(&task->tv, &sched->clock, &tv);
			return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
		}
	}
	return 1000;
}

t_rinootask *rinoo_task_driver_getcurrent(t_rinoosched *sched)
{
	return sched->driver.current;
}

/**
 * Create a new task.
 *
 * @param sched sched Pointer to a scheduler to use
 * @param function Routine to call for that task
 * @param arg Routine argument to be passed
 *
 * @return Pointer to the created task, or NULL if an error occurs
 */
t_rinootask *rinoo_task(t_rinoosched *sched, t_rinootask *parent, void (*function)(void *arg), void *arg)
{
	t_rinootask *task;

	XASSERT(sched != NULL, NULL);
	XASSERT(function != NULL, NULL);

	task = malloc(sizeof(*task));
	if (task == NULL) {
		return NULL;
	}
	task->sched = sched;
	task->scheduled = false;
	task->context.stack.sp = task->stack;
	task->context.stack.size = sizeof(task->stack);
	task->context.link = NULL;
	if (parent != NULL) {
		task->context.link = &parent->context;
	}
	memset(&task->tv, 0, sizeof(task->tv));
	memset(&task->proc_node, 0, sizeof(task->proc_node));
	fcontext(&task->context, function, arg);
	return task;
}

void rinoo_task_destroy(t_rinootask *task)
{
	rinoo_task_unschedule(task);
	free(task);
}

int rinoo_task_start(t_rinoosched *sched, void (*function)(void *arg), void *arg)
{
	t_rinootask *task;

	task = rinoo_task(sched, &sched->driver.main, function, arg);
	if (task == NULL) {
		return -1;
	}
	rinoo_task_schedule(task, NULL);
	return 0;
}

int rinoo_task_run(t_rinoosched *sched, void (*function)(void *arg), void *arg)
{
	t_rinootask *task;

	task = rinoo_task(sched, sched->driver.current, function, arg);
	if (task == NULL) {
		return -1;
	}
	return rinoo_task_resume(task);
}

/**
 * Resume a task.
 * This function switches to the task stack by calling fcontext_swap.
 *
 * @param task Pointer to the task to run or resume
 *
 * @return 1 if the given task has been executed and is over, 0 if it's been released, -1 if an error occurs
 */
int rinoo_task_resume(t_rinootask *task)
{
	int ret;
	t_rinootask *old;
	t_rinootask_driver *driver;

	XASSERT(task != NULL, -1);

	driver = &task->sched->driver;
	old = driver->current;
	driver->current = task;

#ifdef RINOO_DEBUG
	/* This code avoids valgrind to mix stack switches */
	int valgrind_stackid = VALGRIND_STACK_REGISTER(task->stack, task->stack + sizeof(task->stack));
#endif /* !RINOO_DEBUG */

	ret = fcontext_swap(&old->context, &task->context);

#ifdef RINOO_DEBUG
	VALGRIND_STACK_DEREGISTER(valgrind_stackid);
#endif /* !RINOO_DEBUG */

	driver->current = old;
	if (ret == 0) {
		/* This task is finished */
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

	errno = 0;
	if (fcontext_swap(&sched->driver.current->context, &sched->driver.main.context) != 1) {
		return -1;
	}
	return errno;
}

int rinoo_task_schedule(t_rinootask *task, struct timeval *tv)
{
	XASSERT(task != NULL, -1);
	XASSERT(task->sched != NULL, -1);

	if (task->scheduled == true) {
		rinoorbtree_remove(&task->sched->driver.proc_tree, &task->proc_node);
		task->scheduled = false;
	}
	if (tv != NULL) {
		task->tv = *tv;
	} else {
		memset(&task->tv, 0, sizeof(task->tv));
	}
	if (rinoorbtree_put(&task->sched->driver.proc_tree, &task->proc_node) != 0) {
		return -1;
	}
	task->scheduled = true;
	return 0;
}

int rinoo_task_unschedule(t_rinootask *task)
{
	XASSERT(task != NULL, -1);
	XASSERT(task->sched != NULL, -1);

	if (task->scheduled == true) {
		rinoorbtree_remove(&task->sched->driver.proc_tree, &task->proc_node);
		memset(&task->tv, 0, sizeof(task->tv));
		task->scheduled = false;
	}
	return 0;
}
