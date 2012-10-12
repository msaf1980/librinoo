#include "rinoo/rinoo.h"

int rinoo_context(t_fcontext *ctx, void (*func)(void *ptr), void *arg)
{
	unsigned long int *sp;

	sp = (unsigned long int *)(ctx->stack.sp + ctx->stack.size);
	sp--;
	sp = (unsigned long int *) ((((uintptr_t) sp) & -16L) - 8);
	sp[0] = (unsigned long int) &rinoo_context_jump;
	sp[1] = (unsigned long int) ctx->link;
	ctx->reg[FREG_RIP] = (long int) func;
	ctx->reg[FREG_RDI] = (long int) arg;
	ctx->reg[FREG_RBX] = (long int) &sp[1];
	ctx->reg[FREG_RSP] = (long int) sp;
	return 0;
}
