
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
	
	push	64(%rsi) 
	mov		32(%rsi),   %RSI

	ret 

.globl ini_context
ini_context:

	ret 
	
	
