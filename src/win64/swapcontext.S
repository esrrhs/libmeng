.text
.globl swap_context
.globl _swap_context
#if defined(__ELF__)
.type swap_context, @function
#elif defined(_WIN32)
.def swap_context; .scl 2; .type 32; .endef
#endif
swap_context:
_swap_context:
	/* rcx = old_context, rdx = new_context */

	/* Save callee-saved non-volatile general-purpose registers */
	movq	%rbx, 0(%rcx)
	movq	%rbp, 8(%rcx)
	movq	%rsi, 16(%rcx)
	movq	%rdi, 24(%rcx)
	movq	%r12, 32(%rcx)
	movq	%r13, 40(%rcx)
	movq	%r14, 48(%rcx)
	movq	%r15, 56(%rcx)

	/* Save stack pointer (caller RSP was %rsp + 8 before call) */
	leaq	8(%rsp), %rax
	movq	%rax, 64(%rcx)

	/* Save return address (RIP) */
	movq	(%rsp), %rax
	movq	%rax, 72(%rcx)

	/* Save callee-saved non-volatile XMM registers (xmm6-xmm15) */
	movdqu	%xmm6,  80(%rcx)
	movdqu	%xmm7,  96(%rcx)
	movdqu	%xmm8,  112(%rcx)
	movdqu	%xmm9,  128(%rcx)
	movdqu	%xmm10, 144(%rcx)
	movdqu	%xmm11, 160(%rcx)
	movdqu	%xmm12, 176(%rcx)
	movdqu	%xmm13, 192(%rcx)
	movdqu	%xmm14, 208(%rcx)
	movdqu	%xmm15, 224(%rcx)

	/* Restore from new_context (rdx) */
	movq	0(%rdx),  %rbx
	movq	8(%rdx),  %rbp
	movq	16(%rdx), %rsi
	movq	24(%rdx), %rdi
	movq	32(%rdx), %r12
	movq	40(%rdx), %r13
	movq	48(%rdx), %r14
	movq	56(%rdx), %r15

	movdqu	80(%rdx),  %xmm6
	movdqu	96(%rdx),  %xmm7
	movdqu	112(%rdx), %xmm8
	movdqu	128(%rdx), %xmm9
	movdqu	144(%rdx), %xmm10
	movdqu	160(%rdx), %xmm11
	movdqu	176(%rdx), %xmm12
	movdqu	192(%rdx), %xmm13
	movdqu	208(%rdx), %xmm14
	movdqu	224(%rdx), %xmm15

	/* Restore stack pointer */
	movq	64(%rdx), %rsp

	/* Push target RIP */
	pushq	72(%rdx)

	retq

.globl ini_context
.globl _ini_context
#if defined(__ELF__)
.type ini_context, @function
#elif defined(_WIN32)
.def ini_context; .scl 2; .type 32; .endef
#endif
ini_context:
_ini_context:
	retq
