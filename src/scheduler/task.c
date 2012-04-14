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
	if (timercmp(&task1->tv, &task2->tv, <=)) {
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

	sched->driver.proc_tree = rinoorbtree(rinoo_task_cmp, NULL);
	if (sched->driver.proc_tree == NULL) {
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

	rinoorbtree_destroy(sched->driver.proc_tree);
	sched->driver.proc_tree = NULL;
}

u32 rinoo_task_driver_run(t_rinoosched *sched)
{
	t_rinootask *task;
	struct timeval tv;
	t_rinoorbtree_node *head;

	XASSERT(sched != NULL, 1000);

	while ((head = rinoorbtree_head(sched->driver.proc_tree)) != NULL) {
		task = container_of(head, t_rinootask, proc_node);
		if (timercmp(&task->tv, &sched->clock, <=)) {
			rinoo_task_unschedule(task);
			rinoo_task_run(task);
		} else {
			timersub(&task->tv, &sched->clock, &tv);
			return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
		}
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
	XASSERTN(task->function != NULL);
	task->function(task);
	/* That means the task is over */
	task->function = NULL;
}

/**
 * Initialize a task.
 *
 * @param sched Pointer to a scheduler to use
 * @param task Pointer to the task to initialize
 * @param function Routine to call for that task
 *
 * @return 0 if the task has been successfuly initialize, otherwise -1
 */
int rinoo_task(t_rinoosched *sched, t_rinootask *task, t_rinootask_func function)
{
	XASSERT(sched != NULL, -1);
	XASSERT(function != NULL, -1);

	if (getcontext(&task->context) != 0) {
		return -1;
	}
	task->sched = sched;
	task->queued = FALSE;
	task->function = function;
	task->context.uc_stack.ss_sp = task->stack;
	task->context.uc_stack.ss_size = sizeof(task->stack);
	task->context.uc_link = NULL;
	memset(&task->tv, 0, sizeof(task->tv));
	memset(&task->proc_node, 0, sizeof(task->proc_node));
	return 0;
}

/**
 * Run or resume a task.
 * This function switches to the task stack by calling swapcontext(3).
 *
 * @param task Pointer to the task to run or resume
 *
 * @return 1 if the given task has been executed and is over, 0 if it's been released, -1 if an error occurs
 */
int rinoo_task_run(t_rinootask *task)
{
	int ret;
	t_rinootask *old;
	t_rinootask_driver *driver;

	XASSERT(task != NULL, -1);

	if (task->context.uc_link == NULL) {
		t_rinootask_arg targ = {.ptr = task};
		task->context.uc_link = &(task->sched->driver.current->context);
		makecontext(&task->context, (void (*)()) rinoo_task_process, 2, targ.args[0], targ.args[1]);
	}
	driver = &task->sched->driver;
	old = driver->current;
	driver->current = task;

#ifdef RINOO_DEBUG
	/* This code avoids valgrind to mix stack switches */
	int valgrind_stackid = VALGRIND_STACK_REGISTER(task->stack, task->stack + sizeof(task->stack));
#endif /* !RINOO_DEBUG */

	ret = swapcontext(&old->context, &task->context);

#ifdef RINOO_DEBUG
	VALGRIND_STACK_DEREGISTER(valgrind_stackid);
#endif /* !RINOO_DEBUG */

	driver->current = old;
	if (ret == 0 && task->function == NULL) {
		/* This task is finished */
		rinoo_task_unschedule(task);
		return 1;
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
	if (swapcontext(&sched->driver.current->context,
			&sched->driver.main.context) != 0) {
		return -1;
	}
	return errno;
}

int rinoo_task_schedule(t_rinootask *task, struct timeval *tv)
{
	XASSERT(task != NULL, -1);
	XASSERT(task->sched != NULL, -1);

	if (task->queued == TRUE) {
		rinoorbtree_remove(task->sched->driver.proc_tree, &task->proc_node);
		task->queued = FALSE;
	}
	if (tv != NULL) {
		task->tv = *tv;
	} else {
		memset(&task->tv, 0, sizeof(task->tv));
	}
	if (rinoorbtree_put(task->sched->driver.proc_tree, &task->proc_node) != 0) {
		return -1;
	}
	task->queued = TRUE;
	return 0;
}

int rinoo_task_unschedule(t_rinootask *task)
{
	XASSERT(task != NULL, -1);
	XASSERT(task->sched != NULL, -1);

	if (task->queued == TRUE) {
		rinoorbtree_remove(task->sched->driver.proc_tree, &task->proc_node);
		memset(&task->tv, 0, sizeof(task->tv));
		task->queued = FALSE;
	}
	return 0;
}
