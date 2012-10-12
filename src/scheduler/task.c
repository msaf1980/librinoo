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
static void rinoo_task_process(void *arg)
{
	t_rinootask *task = arg;

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
int rinoo_task(t_rinoosched *sched,
	       t_rinootask *task,
	       void (*function)(t_rinootask *task),
	       void (*delete)(t_rinootask *task))
{
	XASSERT(sched != NULL, -1);
	XASSERT(function != NULL, -1);

	if (rinoo_context_get(&task->context) != 0) {
		return -1;
	}
	task->sched = sched;
	task->queued = FALSE;
	task->function = function;
	task->delete = delete;
	task->context.stack.sp = task->stack;
	task->context.stack.size = sizeof(task->stack);
	task->context.link = NULL;
	memset(&task->tv, 0, sizeof(task->tv));
	memset(&task->proc_node, 0, sizeof(task->proc_node));
	return 0;
}

/* int myswap(ucontext_t *oucp, ucontext_t *ucp) */
/* { */
/* 	asm("mov    %%rbp, %%rsp\t\n" */
/* 	    "pop    %%rbp\t\n" */
/* 	    "mov    %%rbx,0x80(%0)\t\n" */
/* 	    "mov    %%rbp,0x78(%0)\t\n" */
/* 	    "mov    %%r12,0x48(%0)\t\n" */
/* 	    "mov    %%r13,0x50(%0)\t\n" */
/* 	    "mov    %%r14,0x58(%0)\t\n" */
/* 	    "mov    %%r15,0x60(%0)\t\n" */
/* 	    "mov    %%rdi,0x68(%0)\t\n" */
/* 	    "mov    %%rsi,0x70(%0)\t\n" */
/* 	    "mov    %%rdx,0x88(%0)\t\n" */
/* 	    "mov    %%rcx,0x98(%0)\t\n" */
/* 	    "mov    %%r8,0x28(%0)\t\n" */
/* 	    "mov    %%r9,0x30(%0)\t\n" */
/* 	    "mov    (%%rsp),%%rcx\t\n" */
/* 	    "mov    %%rcx,0xa8(%0)\t\n" */
/* 	    "lea    0x8(%%rsp),%%rcx\t\n" */
/* 	    "mov    %%rcx,0xa0(%0)\t\n" */

/* 	    "lea    0x1a8(%0),%%rcx\t\n" */
/* 	    "mov    %%rcx,0xe0(%0)\t\n" */
/* 	    /\* "fnstenv (%rcx)\t\n" *\/ */
/* 	    /\* "stmxcsr 0x1c0(%rdi)\t\n" *\/ */
/* 	    /\* "mov    %rsi,%r12\t\n" *\/ */
/* 	    /\* "lea    0x128(%rdi),%rdx\t\n" *\/ */
/* 	    /\* "lea    0x128(%rsi),%rsi\t\n" *\/ */
/* 	    /\* "mov    $0x2,%edi\t\n" *\/ */
/* 	    /\* "mov    $0x8,%r10d\t\n" *\/ */
/* 	    /\* "mov    $0xe,%eax\t\n" *\/ */
/* 	    /\* "syscall\t\n" *\/ */
/* 	    /\* "cmp    $0xfffffffffffff001,%rax\t\n" *\/ */
/* 	    /\* "jae    46490 <swapcontext+0xf0>\t\n" *\/ */
/* 	    /\* "mov    %r12,%rsi\t\n" *\/ */
/* 	    /\* "mov    0xe0(%rsi),%rcx\t\n" *\/ */
/* 	    /\* "fldenv (%rcx)\t\n" *\/ */
/* 	    /\* "ldmxcsr 0x1c0(%rsi)\t\n" *\/ */
/* 	    "mov    0xa0(%1),%%rsp\t\n" */
/* 	    "mov    0x80(%1),%%rbx\t\n" */
/* 	    "mov    0x78(%1),%%rbp\t\n" */
/* 	    "mov    0x48(%1),%%r12\t\n" */
/* 	    "mov    0x50(%1),%%r13\t\n" */
/* 	    "mov    0x58(%1),%%r14\t\n" */
/* 	    "mov    0x60(%1),%%r15\t\n" */
/* 	    "mov    0xa8(%1),%%rcx\t\n" */
/* 	    "push   %%rcx\t\n" */
/* 	    "mov    0x68(%1),%%rdi\t\n" */
/* 	    "mov    0x88(%1),%%rdx\t\n" */
/* 	    "mov    0x98(%1),%%rcx\t\n" */
/* 	    "mov    0x28(%1),%%r8\t\n" */
/* 	    "mov    0x30(%1),%%r9\t\n" */
/* 	    "mov    0x70(%1),%%rsi\t\n" */
/* 	    "xor    %%eax,%%eax\t\n" */
/* 	    "retq\t\n" */
/* 	    : */
/* 	    :"D" (oucp), "S" (ucp) */
/* 	    :"memory" */
/* 	    /\* "mov    0x371989(%rip),%rcx\t\n" *\/ */
/* 	    /\* "xor    %edx,%edx\t\n" *\/ */
/* 	    /\* "sub    %rax,%rdx\t\n" *\/ */
/* 	    /\* "mov    %edx,%fs:(%rcx)\t\n" *\/ */
/* 	    /\* "or     $0xffffffffffffffff,%rax\t\n" *\/ */
/* 	    /\* "jmp    4648f <swapcontext+0xef>" *\/ */
/* 		); */
/* 	return 0; */
/* } */

/* int myswap3(ucontext_t *oucp, ucontext_t *ucp) */
/* { */
/* 	asm("mov    %%rbp, %%rsp	\n\ */
/* 	    pop    %%rbp		\n\ */
/* 	    mov    %%rbx,0x80(%0)	\n\ */
/* 	    mov    %%rbp,0x78(%0)	\n\ */
/* 	    mov    %%r12,0x48(%0)	\n\ */
/* 	    mov    %%r13,0x50(%0)	\n\ */
/* 	    mov    %%r14,0x58(%0)	\n\ */
/* 	    mov    %%r15,0x60(%0)	\n\ */
/* 	    mov    %%rdi,0x68(%0)	\n\ */
/* 	    mov    %%rsi,0x70(%0)	\n\ */
/* 	    mov    %%rdx,0x88(%0)	\n\ */
/* 	    mov    %%rcx,0x98(%0)	\n\ */
/* 	    mov    %%r8,0x28(%0)	\n\ */
/* 	    mov    %%r9,0x30(%0)	\n\ */
/* 	    mov    (%%rsp),%%rcx	\n\ */
/* 	    mov    %%rcx,0xa8(%0)	\n\ */
/* 	    lea    0x8(%%rsp),%%rcx	\n\ */
/* 	    mov    %%rcx,0xa0(%0)	\n\ */
/* 	    lea    0x1a8(%0),%%rcx	\n\ */
/* 	    mov    %%rcx,0xe0(%0)	\n\ */
/* 	    mov    0xa0(%1),%%rsp	\n\ */
/* 	    mov    0x80(%1),%%rbx	\n\ */
/* 	    mov    0x78(%1),%%rbp	\n\ */
/* 	    mov    0x48(%1),%%r12	\n\ */
/* 	    mov    0x50(%1),%%r13	\n\ */
/* 	    mov    0x58(%1),%%r14	\n\ */
/* 	    mov    0x60(%1),%%r15	\n\ */
/* 	    mov    0xa8(%1),%%rcx	\n\ */
/* 	    push   %%rcx		\n\ */
/* 	    mov    0x68(%1),%%rdi	\n\ */
/* 	    mov    0x88(%1),%%rdx	\n\ */
/* 	    mov    0x98(%1),%%rcx	\n\ */
/* 	    mov    0x28(%1),%%r8	\n\ */
/* 	    mov    0x30(%1),%%r9	\n\ */
/* 	    mov    0x70(%1),%%rsi	\n\ */
/* 	    xor    %%eax,%%eax		\n\ */
/* 	    retq" */
/* 	    : */
/* 	    :"D" (oucp), "S" (ucp) */
/* 	    :"memory"); */
/* 	return 0; */
/* } */

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

	if (task->context.link == NULL) {
		task->context.link = &(task->sched->driver.current->context);
		rinoo_context(&task->context, rinoo_task_process, task);
	}
	driver = &task->sched->driver;
	old = driver->current;
	driver->current = task;

#ifdef RINOO_DEBUG
	/* This code avoids valgrind to mix stack switches */
	int valgrind_stackid = VALGRIND_STACK_REGISTER(task->stack, task->stack + sizeof(task->stack));
#endif /* !RINOO_DEBUG */

	ret = rinoo_context_swap(&old->context, &task->context);

#ifdef RINOO_DEBUG
	VALGRIND_STACK_DEREGISTER(valgrind_stackid);
#endif /* !RINOO_DEBUG */

	driver->current = old;
	if (ret == 0 && task->function == NULL) {
		/* This task is finished */
		rinoo_task_unschedule(task);
		if (task->delete != NULL) {
			task->delete(task);
		}
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
	if (rinoo_context_swap(&sched->driver.current->context,
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
		rinoorbtree_remove(&task->sched->driver.proc_tree, &task->proc_node);
		task->queued = FALSE;
	}
	if (tv != NULL) {
		task->tv = *tv;
	} else {
		memset(&task->tv, 0, sizeof(task->tv));
	}
	if (rinoorbtree_put(&task->sched->driver.proc_tree, &task->proc_node) != 0) {
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
		rinoorbtree_remove(&task->sched->driver.proc_tree, &task->proc_node);
		memset(&task->tv, 0, sizeof(task->tv));
		task->queued = FALSE;
	}
	return 0;
}
