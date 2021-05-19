/**
 * @file   task.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Scheduler task functions
 *
 *
 */

#include "rinoo/scheduler/module.h"

#ifdef RINOO_DEBUG
# include <valgrind/valgrind.h>
#endif

static __thread rn_task_t *current_task = NULL;

static int rn_task_cmp(rn_rbtree_node_t *node1, rn_rbtree_node_t *node2)
{
	rn_task_t *task1 = container_of(node1, rn_task_t, proc_node);
	rn_task_t *task2 = container_of(node2, rn_task_t, proc_node);

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
int rn_task_driver_init(rn_sched_t *sched)
{
	XASSERT(sched != NULL, -1);

	if (rn_rbtree(&sched->driver.proc_tree, rn_task_cmp, NULL) != 0) {
		return -1;
	}
	sched->driver.main.sched = sched;
	sched->driver.current = &sched->driver.main;
	current_task = &sched->driver.main;
	return 0;
}

/**
 * Destroy internal task driver from a scheduler.
 *
 * @param sched Pointer to the scheduler to use
 */
void rn_task_driver_destroy(rn_sched_t *sched)
{
	XASSERTN(sched != NULL);

	rn_rbtree_flush(&sched->driver.proc_tree);
}

/**
 * Runs pending tasks and returns time before next task (in ms).
 * If no task is queued, -1 is returned.
 *
 * @param sched Pointer to the scheduler to use
 *
 * @return Time before next task in ms or -1 if no task is queued
 */
int rn_task_driver_run(rn_sched_t *sched)
{
	rn_task_t *task;
	struct timeval tv;
	rn_rbtree_node_t *head;

	XASSERT(sched != NULL, -1);

	while ((head = rn_rbtree_head(&sched->driver.proc_tree)) != NULL) {
		task = container_of(head, rn_task_t, proc_node);
		if (timercmp(&task->tv, &sched->clock, <=)) {
			rn_task_unschedule(task);
			rn_task_resume(task);
		} else {
			timersub(&task->tv, &sched->clock, &tv);
			return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
		}
	}
	return -1;
}

/**
 * Attempts to stop all pending tasks
 *
 * @param sched Pointer to the scheduler to use
 *
 * @return 0 on success otherwise -1 if an error occurs
 */
int rn_task_driver_stop(rn_sched_t *sched)
{
	rn_task_t *task;
	rn_rbtree_node_t *head;

	XASSERT(sched != NULL, -1);
	XASSERT(sched->stop == true, -1);

	while ((head = rn_rbtree_head(&sched->driver.proc_tree)) != NULL) {
		task = container_of(head, rn_task_t, proc_node);
		rn_task_unschedule(task);
		rn_task_resume(task);
	}
	return 0;
}

/**
 * Returns number of pending tasks.
 *
 * @param sched Pointer to the schedulter to use
 *
 * @return Number of pending tasks.
 */
uint32_t rn_task_driver_nbpending(rn_sched_t *sched)
{
	return sched->driver.proc_tree.size;
}

/**
 * Gets current running task.
 *
 * @param sched Pointer to the scheduler to use
 *
 * @return Pointer to the current task
 */
rn_task_t *rn_task_driver_getcurrent(rn_sched_t *sched)
{
	return sched->driver.current;
}

#if defined(RINOO_JUMP_BOOST)
typedef struct {
        rn_task_t *from;
        rn_task_t *to;
} s_jump_t;

static int fcontext_swap(rn_task_t *from, rn_task_t *to)
{
        s_jump_t jump, *jmp;
        transfer_t t;

        jump.from  = from;
        jump.to    = to;
        t      = jump_fcontext(to->transfer.fctx, &jump);
        jmp    = (s_jump_t*) t.data;
        jmp->from->transfer = t;

		return to->active;
}

static void _task_start(transfer_t t)
{
	s_jump_t *jmp = (s_jump_t*) t.data;
	jmp->from->transfer = t;
	jmp->to->start_func(jmp->to->arg);
	jmp->to->active = 0;
	jump_fcontext(t.fctx, t.data);
}
#endif


/**
 * Create a new task.
 *
 * @param sched sched Pointer to a scheduler to use
 * @param function Routine to call for that task
 * @param arg Routine argument to be passed
 *
 * @return Pointer to the created task, or NULL if an error occurs
 */
rn_task_t *rn_task(rn_sched_t *sched, rn_task_t *parent, void (*function)(void *arg), void *arg)
{
	rn_task_t *task;

	XASSERT(sched != NULL, NULL);
	XASSERT(parent != NULL, NULL);
	XASSERT(function != NULL, NULL);

	task = malloc(sizeof(*task));
	if (task == NULL) {
		return NULL;
	}
	task->sched = sched;
	task->scheduled = false;
	memset(&task->tv, 0, sizeof(task->tv));
	memset(&task->proc_node, 0, sizeof(task->proc_node));

#if defined(RINOO_JUMP_BOOST)
	task->active = 1;
	task->start_func = function;
	task->arg = arg;
	task->parent = parent;
	task->transfer.fctx = make_fcontext(task->stack + sizeof(task->stack), sizeof(task->stack),
                (void(*)(transfer_t)) _task_start);
# elif defined(RINOO_JUMP_FCONTEXT)
	task->fctx.stack.sp = task->stack;
	task->fctx.stack.size = sizeof(task->stack);
	task->fctx.parent = &parent->fctx;
	fcontext(&task->fctx, function, arg);
#else
	#error unhandled RINOO_CONTEXT type
#endif		

#ifdef RINOO_DEBUG
	/* This code avoids valgrind to mix stack switches */
	task->valgrind_stackid = VALGRIND_STACK_REGISTER(task->stack, task->stack + sizeof(task->stack));
#endif /* !RINOO_DEBUG */

	return task;
}

/**
 * Destroy a task.
 *
 * @param task Pointer to the task to destroy
 */
void rn_task_destroy(rn_task_t *task)
{
	XASSERTN(task != NULL);

#ifdef RINOO_DEBUG
	VALGRIND_STACK_DEREGISTER(task->valgrind_stackid);
#endif /* !RINOO_DEBUG */
	rn_task_unschedule(task);
	free(task);
}

/**
 * Queue a task to be launch asynchronously.
 *
 * @param sched Pointer to the scheduler to use
 * @param function Pointer to the routine function
 * @param arg Argument to be passed to the routine function
 *
 * @return 0 on success, otherwise -1
 */
int rn_task_start(rn_sched_t *sched, void (*function)(void *arg), void *arg)
{
	rn_task_t *task;

	task = rn_task(sched, &sched->driver.main, function, arg);
	if (task == NULL) {
		return -1;
	}
	rn_task_schedule(task, NULL);
	return 0;
}

/**
 * Run a task within the current task.
 * This function will return once the routine returned.
 *
 * @param sched Pointer to the scheduler to use
 * @param function Pointer to the routine function
 * @param arg Argument to be passed to the routine function
 *
 * @return 0 on success, otherwise -1
 */
int rn_task_run(rn_sched_t *sched, void (*function)(void *arg), void *arg)
{
	rn_task_t *task;

	task = rn_task(sched, sched->driver.current, function, arg);
	if (task == NULL) {
		return -1;
	}
	return rn_task_resume(task);
}

/**
 * Resume a task.
 * This function switches to the task stack by calling fcontext_swap.
 *
 * @param task Pointer to the task to run or resume
 *
 * @return 1 if the given task has been executed and is over, 0 if it's been released, -1 if an error occurs
 */
int rn_task_resume(rn_task_t *task)
{
	int ret;
	rn_task_t *old;
	rn_task_driver_t *driver;

	XASSERT(task != NULL, -1);

	driver = &task->sched->driver;
	old = driver->current;
	driver->current = task;
	current_task = task;

#if defined(RINOO_JUMP_BOOST)
	ret = fcontext_swap(old, task);
# elif defined(RINOO_JUMP_FCONTEXT)
	ret = fcontext_swap(&old->fctx, &task->fctx);
#else
	#error unhandled RINOO_CONTEXT type
#endif	

	driver->current = old;
	current_task = old;
	if (ret == 0) {
		/* This task is finished */
		rn_task_destroy(task);
	}
	return ret;
}

/**
 * Release execution of a task currently running on a scheduler.
 *
 * @param sched Pointer to the scheduler to use
 *
 * @return 0 on success or -1 if an error occurs
 */
int rn_task_release(rn_sched_t *sched)
{
	XASSERT(sched != NULL, -1);

#if defined(RINOO_JUMP_BOOST)
	fcontext_swap(sched->driver.current, &sched->driver.main);
# elif defined(RINOO_JUMP_FCONTEXT)
	fcontext_swap(&sched->driver.current->fctx, &sched->driver.main.fctx);
#else
	#error unhandled RINOO_CONTEXT type
#endif

	if (sched->stop == true) {
		rn_error_set(ECANCELED);
		return -1;
	}
	return 0;
}

/**
 * Schedule a task to be executed at specific time.
 *
 * @param task Pointer to the task to schedule
 * @param tv Pointer to a timeval representing the expected execution time
 *
 * @return 0 on success or -1 if an error occurs
 */
int rn_task_schedule(rn_task_t *task, struct timeval *tv)
{
	XASSERT(task != NULL, -1);
	XASSERT(task->sched != NULL, -1);

	if (task->scheduled == true) {
		rn_rbtree_remove(&task->sched->driver.proc_tree, &task->proc_node);
		task->scheduled = false;
	}
	if (tv != NULL) {
		task->tv = *tv;
	} else {
		memset(&task->tv, 0, sizeof(task->tv));
	}
	if (rn_rbtree_put(&task->sched->driver.proc_tree, &task->proc_node) != 0) {
		return -1;
	}
	task->scheduled = true;
	return 0;
}

/**
 * Remove a task which has been scheduled for execution.
 *
 * @param task Pointer to the task to unschedule
 *
 * @return 0 on success or -1 if an error occurs
 */
int rn_task_unschedule(rn_task_t *task)
{
	XASSERT(task != NULL, -1);
	XASSERT(task->sched != NULL, -1);

	if (task->scheduled == true) {
		rn_rbtree_remove(&task->sched->driver.proc_tree, &task->proc_node);
		memset(&task->tv, 0, sizeof(task->tv));
		task->scheduled = false;
	}
	return 0;
}

/**
 * Release a task for a given time.
 *
 * @param sched Pointer to the scheduler to use
 * @param ms Release time in milliseconds
 *
 * @return 0 on success or -1 if an error occurs
 */
int rn_task_wait(rn_sched_t *sched, uint32_t ms)
{
	struct timeval res;
	struct timeval toadd;

	if (ms == 0) {
		if (rn_task_schedule(rn_task_driver_getcurrent(sched), NULL) != 0) {
			return -1;
		}
	} else {
		toadd.tv_sec = ms / 1000;
		toadd.tv_usec = (ms % 1000) * 1000;
		timeradd(&sched->clock, &toadd, &res);
		if (rn_task_schedule(rn_task_driver_getcurrent(sched), &res) != 0) {
			return -1;
		}
	}
	return rn_task_release(sched);
}

/**
 * Release a task to be re-scheduled as soon as possible.
 * This can be called by a busy task to give processing back to the scheduler.
 *
 * @param sched Pointer to the scheduler to use
 *
 * @return 0 on success or -1 if an error occurs
 */
int rn_task_pause(rn_sched_t *sched)
{
	rn_task_t *task;
	struct timeval tv;

	task = rn_task_driver_getcurrent(sched);
	if (task == &sched->driver.main) {
		return 0;
	}
	if (task->scheduled == true) {
		tv = task->tv;
		if (rn_task_schedule(task, NULL) != 0) {
			return -1;
		}
		if (rn_task_release(sched) != 0) {
			return -1;
		}
		if (rn_task_schedule(task, &tv) != 0) {
			return -1;
		}
	} else {
		if (rn_task_schedule(task, NULL) != 0) {
			return -1;
		}
		if (rn_task_release(sched) != 0) {
			return -1;
		}
	}
	return 0;
}

/**
 * Gets current running task.
 *
 *
 * @return Pointer to the active task or NULL if no task is running
 */
rn_task_t *rn_task_self(void)
{
	return current_task;
}
