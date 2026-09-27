.text
.globl swap_context
.globl _swap_context
#if defined(__ELF__)
.type swap_context, @function
#endif
swap_context:
_swap_context:

	/* rdi = old_context, rsi = new_context */

	/* Save general purpose registers */
	movq	%rax, 0(%rdi)
	movq	%rbx, 8(%rdi)
	movq	%rcx, 16(%rdi)
	movq	%rdx, 24(%rdi)
	movq	%rsi, 32(%rdi)
	movq	%rdi, 40(%rdi)
	
	/* RSP: When swap_context was called, (%rsp) has the return address.
	   The caller's stack pointer before call was (%rsp + 8) */ 
	leaq	8(%rsp), %rcx
	movq	%rcx, 48(%rdi)

	movq	%rbp, 56(%rdi)

	/* Return address (RIP) */ 
	movq	(%rsp), %rcx
	movq	%rcx, 64(%rdi)
	
	movq	%r8,  72(%rdi)
	movq	%r9,  80(%rdi)
	movq	%r10, 88(%rdi)
	movq	%r11, 96(%rdi)
	movq	%r12, 104(%rdi)
	movq	%r13, 112(%rdi)
	movq	%r14, 120(%rdi)
	movq	%r15, 128(%rdi)
	
	/* Save XMM registers */
	movdqu	%xmm0, 136(%rdi)
	movdqu	%xmm1, 152(%rdi)
	movdqu	%xmm2, 168(%rdi)
	movdqu	%xmm3, 184(%rdi)
	movdqu	%xmm4, 200(%rdi)
	movdqu	%xmm5, 216(%rdi)
	movdqu	%xmm6, 232(%rdi)
	movdqu	%xmm7, 248(%rdi)
	
	/* Restore registers from new_context (%rsi) */ 
	movq	0(%rsi),   %rax  
	movq	8(%rsi),   %rbx  
	movq	16(%rsi),  %rcx
	movq	24(%rsi),  %rdx   
	movq	48(%rsi),  %rsp  
	movq	56(%rsi),  %rbp  
	
	movq	72(%rsi),  %r8
	movq	80(%rsi),  %r9
	movq	88(%rsi),  %r10
	movq	96(%rsi),  %r11
	movq	104(%rsi), %r12
	movq	112(%rsi), %r13
	movq	120(%rsi), %r14
	movq	128(%rsi), %r15
	
	/* Restore XMM registers from %rsi */
	movdqu	136(%rsi), %xmm0
	movdqu	152(%rsi), %xmm1
	movdqu	168(%rsi), %xmm2
	movdqu	184(%rsi), %xmm3
	movdqu	200(%rsi), %xmm4
	movdqu	216(%rsi), %xmm5
	movdqu	232(%rsi), %xmm6
	movdqu	248(%rsi), %xmm7
	
	/* Push target RIP */
	pushq	64(%rsi) 

	/* Restore RDI and RSI last so RSI remains valid during earlier reads */
	movq	40(%rsi), %rdi  
	movq	32(%rsi), %rsi

	retq

.globl ini_context
.globl _ini_context
#if defined(__ELF__)
.type ini_context, @function
#endif
ini_context:
_ini_context:
	retq

#if defined(__linux__) && defined(__ELF__)
.section .note.GNU-stack,"",@progbits
#endif
