/*
 * Copyright (c) 2012 Reginald Lips
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions: The above copyright notice and this
 * permission notice shall be included in all copies or substantial
 * portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

.globl fcontext_jump
	.type fcontext_jump, @function

fcontext_jump:
	movq	%rbx, %rsp
	popq	%rdi
	testq	%rdi, %rdi
	je	2f
	movq	0x28(%rdi), %r12
	movq	0x30(%rdi), %r13
	movq	0x38(%rdi), %r14
	movq	0x40(%rdi), %r15
	movq	0x58(%rdi), %rbp
	movq	0x60(%rdi), %rbx
	movq	0x78(%rdi), %rsp
	movq	0x80(%rdi), %rcx
	pushq	%rcx
	movq	0x8(%rdi), %r8
	movq	0x10(%rdi), %r9
	movq	0x50(%rdi), %rsi
	movq	0x68(%rdi), %rdx
	movq	0x70(%rdi), %rcx
	movq	0x48(%rdi), %rdi
	xorl    %eax, %eax
	ret

2:	movq    %rax,%rdi
	call    exit@PLT
	hlt

.globl fcontext_swap
	.type fcontext_swap, @function

fcontext_swap:
	movq	%r8, 0x8(%rdi)
	movq	%r9, 0x10(%rdi)
	movq	%r12, 0x28(%rdi)
	movq	%r13, 0x30(%rdi)
	movq	%r14, 0x38(%rdi)
	movq	%r15, 0x40(%rdi)
	movq	%rdi, 0x48(%rdi)
	movq	%rsi, 0x50(%rdi)
	movq	%rbp, 0x58(%rdi)
	movq	%rbx, 0x60(%rdi)
	movq	%rdx, 0x68(%rdi)
	movq	%rcx, 0x70(%rdi)
	movq	(%rsp), %rcx
	movq	%rcx, 0x80(%rdi)
	leaq	0x8(%rsp), %rcx
	movq	%rcx, 0x78(%rdi)
	movq	0x28(%rsi), %r12
	movq	0x30(%rsi), %r13
	movq	0x38(%rsi), %r14
	movq	0x40(%rsi), %r15
	movq	0x58(%rsi), %rbp
	movq	0x60(%rsi), %rbx
	movq	0x78(%rsi), %rsp
        movq	0x80(%rsi), %rcx
        pushq	%rcx
	movq	0x8(%rsi), %r8
	movq	0x10(%rsi), %r9
	movq	0x48(%rsi), %rdi
	movq	0x68(%rsi), %rdx
	movq	0x70(%rsi), %rcx
	movq	0x50(%rsi), %rsi
	movl	$1, %eax
	ret

.globl fcontext
	.type fcontext, @function

fcontext:
	movq	0x88(%rdi), %rax
	addq	0x90(%rdi), %rax
	movq	fcontext_jump@GOTPCREL(%rip), %r8
	subq	$0x8, %rax
	andq	$-0x10, %rax
	movq	%r8, -0x8(%rax)
	movq	(%rdi), %r8
	leaq	-0x8(%rax), %rcx
	movq	%r8, (%rax)
	movq	%rdx, 0x48(%rdi)
	movq	%rax, 0x60(%rdi)
	movq	%rcx, 0x78(%rdi)
	movq	%rsi, 0x80(%rdi)
	ret

	/* fcontext C code */
	/*
	void fcontext(t_fcontext *ctx, void (*func)(void *ptr), void *arg)
	{
		unsigned long int *sp;

		sp = (unsigned long int *) ((uintptr_t) ctx->stack.sp + ctx->stack.size);
		sp = (unsigned long int *) ((((uintptr_t) (sp - 1)) & -16L) - 8);

		sp[0] = (unsigned long int) &fcontext_jump;
		sp[1] = (unsigned long int) ctx->link;

		ctx->reg[FREG_RIP] = (long int) func;
		ctx->reg[FREG_RDI] = (long int) arg;
		ctx->reg[FREG_RBX] = (long int) &sp[1];
		ctx->reg[FREG_RSP] = (long int) sp;
	}
	*/

