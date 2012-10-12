.globl rinoo_context_get
	.type rinoo_context_get, @function

rinoo_context_get:
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
	leaq	8(%rsp), %rcx
	movq	%rcx, 0x78(%rdi)
	xorl	%eax, %eax
	ret

.globl rinoo_context_set
	.type rinoo_context_set, @function

rinoo_context_set:
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
	movq	0x48(%rdi), %rdi
	movq	0x50(%rdi), %rsi
	movq	0x68(%rdi), %rdx
	movq	0x70(%rdi), %rcx
	xorl    %eax, %eax
	ret

.globl rinoo_context_jump
	.type rinoo_context_jump, @function

rinoo_context_jump:
	movq	%rbx, %rsp
	popq	%rdi
	testq	%rdi, %rdi
	je	2f
	call	rinoo_context_set@PLT

2:	movq    %rax,%rdi
	call    exit@PLT
	hlt

.globl rinoo_context_swap
	.type rinoo_context_swap, @function

rinoo_context_swap:
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
	movq	0x50(%rsi), %rsi
	movq	0x68(%rsi), %rdx
	movq	0x70(%rsi), %rcx
	xorl	%eax, %eax
	ret

.globl rinoo_context
	.type rinoo_context, @function

rinoo_context:
	movq	0x88(%rdi), %rax
	addq	0x90(%rdi), %rax
	movq	rinoo_context_jump@GOTPCREL(%rip), %r8
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
