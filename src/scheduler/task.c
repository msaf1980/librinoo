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

int rinoo_task_driver_init(t_rinoosched *sched)
{
	t_rinootask_driver *driver;

	XASSERT(sched != NULL, -1);

	driver = calloc(1, sizeof(*driver));
	if (driver == NULL) {
		return -1;
	}
	driver->cur_task = &driver->main_task;
	sched->task_driver = driver;
	return 0;
}

void rinoo_task_driver_destroy(t_rinoosched *sched)
{
	XASSERTN(sched != NULL);

	if (sched->task_driver != NULL) {
		free(sched->task_driver);
		sched->task_driver = NULL;
	}
}

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
}

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

int rinoo_task_run(t_rinootask *task)
{
	int ret;
	t_rinootask *old;

	XASSERT(task != NULL, -1);

	old = task->sched->task_driver->cur_task;
	task->sched->task_driver->cur_task = task;
	ret = swapcontext(&old->context, &task->context);
	task->sched->task_driver->cur_task = old;
	rinoo_task_destroy(task);
	return ret;
}

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

void rinoo_task_destroy(t_rinootask *task)
{
	XASSERTN(task != NULL);

#ifdef RINOO_DEBUG
	VALGRIND_STACK_DEREGISTER(task->valgrind_id);
#endif /* !RINOO_DEBUG */

	free(task);
}
