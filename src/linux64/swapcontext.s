
.globl swap_context
swap_context:

	/* rdi, rsi */

	/* save */
	mov		%RAX, (%rdi)
	mov		%RBX, 8(%rdi)
	mov		%RCX, 16(%rdi)
	mov		%RDX, 24(%rdi)
	mov		%RSI, 32(%rdi)
	mov		%RDI, 40(%rdi)
	
	/* RSP */ 
	lea		8(%rsp),	%RCX
	mov		%RCX, 48(%rdi)

	mov		%RBP, 56(%rdi)

	/* EIP */ 
	mov		(%rsp),	%RCX
	mov		%RCX, 64(%rdi)
	
	mov		%r8, 72(%rdi)
	mov		%r9, 80(%rdi)
	mov		%r10, 88(%rdi)
	mov		%r11, 96(%rdi)
	mov		%r12, 104(%rdi)
	mov		%r13, 112(%rdi)
	mov		%r14, 120(%rdi)
	mov		%r15, 128(%rdi)
	
	movups	%xmm0,  136(%rdi)
	movups	%xmm1,  152(%rdi)
	movups	%xmm2,  168(%rdi)
	movups	%xmm3,  184(%rdi)
	movups	%xmm4,  200(%rdi)
	movups	%xmm5,  216(%rdi)
	movups	%xmm6,  232(%rdi)
	movups	%xmm7,  248(%rdi)
	
	/* load */ 
	mov		(%rsi),		%RAX  
	mov		8(%rsi),    %RBX  
	mov		16(%rsi),	%RCX
	mov		24(%rsi),   %RDX   
	mov		40(%rsi),   %RDI  
	mov		48(%rsi),   %RSP  
	mov		56(%rsi),   %RBP  
	
	mov		72(%rsi),	%r8
	mov		80(%rsi),   %r9
	mov		88(%rsi),   %r10
	mov		96(%rsi),   %r11
	mov		104(%rsi),  %r12
	mov		112(%rsi),  %r13
	mov		120(%rsi),  %r14
	mov		128(%rsi),  %r15
	
	movups	136(%rdi),  %xmm0
	movups	152(%rdi),  %xmm1
	movups	168(%rdi),  %xmm2
	movups	184(%rdi),  %xmm3
	movups	200(%rdi),  %xmm4
	movups	216(%rdi),  %xmm5
	movups	232(%rdi),  %xmm6
	movups	248(%rdi),  %xmm7
	
	push	64(%rsi) 
	mov		32(%rsi),   %RSI

	ret 

.globl ini_context
ini_context:

	ret 
	
.globl get_meng
get_meng:

	mov		8(%rbp),	%rax
	ret 

