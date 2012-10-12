/**
 * @file   fcontext.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Wed Oct 10 15:18:43 2012
 *
 * @brief  Header file for fast context.
 *
 *
 */

#ifndef		RINOO_SCHEDULER_FCONTEXT_H_
# define	RINOO_SCHEDULER_FCONTEXT_H_

enum
{
	FREG_R8 = 0,
	FREG_R9,
	FREG_R10,
	FREG_R11,
	FREG_R12,
	FREG_R13,
	FREG_R14,
	FREG_R15,
	FREG_RDI,
	FREG_RSI,
	FREG_RBP,
	FREG_RBX,
	FREG_RDX,
	FREG_RCX,
	FREG_RSP,
	FREG_RIP,
	NB_FREG
};

typedef struct s_fstack
{
	void *sp;
	size_t size;
} t_fstack;

typedef struct s_fcontext
{
	struct s_fcontext *link;
	long int reg[NB_FREG];
	t_fstack stack;
} t_fcontext;

int rinoo_context(t_fcontext *ctx, void (*func)(void *ptr), void *arg);
void rinoo_context_jump(void);
int rinoo_context_get(t_fcontext *ctx);
int rinoo_context_set(t_fcontext *ctx);
int rinoo_context_swap(t_fcontext *octx, t_fcontext *nctx);

#endif		/* !RINOO_SCHEDULER_FCONTEXT_H_ */
