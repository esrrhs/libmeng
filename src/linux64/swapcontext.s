
.globl swap_context
swap_context:

	push		%RAX
	mov		16(%rsp),	%RAX

	/* save */
	mov		%RBX, 8(%rax)
	mov		%RCX, 16(%rax)
	mov		%RDX, 24(%rax)
	mov		%RSI, 32(%rax)
	mov		%RDI, 40(%rax)
	
	/* RSP */ 
	lea		16(%rsp),	%RCX
	mov		%RCX, 48(%rax)

	mov		%RBP, 56(%rax)

	/* EIP */ 
	mov		8(%rsp),	%RCX
	mov		%RCX, 64(%rax)
	
	mov		%RAX,	%RCX
	pop		%RAX
	mov		%RAX,	(%RCX)

	mov		16(%rsp),	%RCX

	/* load */ 
	mov		%RAX,  (%rcx)
	mov		%RBX,  8(%rcx)
	mov		%RDX,  24(%rcx)
	mov		%RSI,  32(%rcx)
	mov		%RDI,  40(%rcx)
	mov		%RSP,  48(%rcx)
	mov		%RBP,  56(%rcx)
	
	push	64(%rcx)
	mov		16(%rcx),	%RCX

	ret 

.globl ini_context
ini_context:

	ret 
	
	
