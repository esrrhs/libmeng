.code

swap_context PROC
	; rcx = old_context, rdx = new_context

	; Save callee-saved non-volatile registers
	mov     qword ptr [rcx + 16], rbp
	mov     qword ptr [rcx + 24], rbx
	mov     qword ptr [rcx + 32], rsi
	mov     qword ptr [rcx + 40], rdi
	mov     qword ptr [rcx + 48], r12
	mov     qword ptr [rcx + 56], r13
	mov     qword ptr [rcx + 64], r14
	mov     qword ptr [rcx + 72], r15

	; Save stack pointer (caller rsp is [rsp + 8])
	lea     rax, [rsp + 8]
	mov     qword ptr [rcx + 8], rax

	; Save return address (RIP)
	mov     rax, qword ptr [rsp]
	mov     qword ptr [rcx + 0], rax

	; Save non-volatile XMM registers (xmm6 - xmm15)
	movaps  xmmword ptr [rcx + 80],  xmm6
	movaps  xmmword ptr [rcx + 96],  xmm7
	movaps  xmmword ptr [rcx + 112], xmm8
	movaps  xmmword ptr [rcx + 128], xmm9
	movaps  xmmword ptr [rcx + 144], xmm10
	movaps  xmmword ptr [rcx + 160], xmm11
	movaps  xmmword ptr [rcx + 176], xmm12
	movaps  xmmword ptr [rcx + 192], xmm13
	movaps  xmmword ptr [rcx + 208], xmm14
	movaps  xmmword ptr [rcx + 224], xmm15

	; Restore from new_context (rdx)
	mov     rbp, qword ptr [rdx + 16]
	mov     rbx, qword ptr [rdx + 24]
	mov     rsi, qword ptr [rdx + 32]
	mov     rdi, qword ptr [rdx + 40]
	mov     r12, qword ptr [rdx + 48]
	mov     r13, qword ptr [rdx + 56]
	mov     r14, qword ptr [rdx + 64]
	mov     r15, qword ptr [rdx + 72]

	movaps  xmm6,  xmmword ptr [rdx + 80]
	movaps  xmm7,  xmmword ptr [rdx + 96]
	movaps  xmm8,  xmmword ptr [rdx + 112]
	movaps  xmm9,  xmmword ptr [rdx + 128]
	movaps  xmm10, xmmword ptr [rdx + 144]
	movaps  xmm11, xmmword ptr [rdx + 160]
	movaps  xmm12, xmmword ptr [rdx + 176]
	movaps  xmm13, xmmword ptr [rdx + 192]
	movaps  xmm14, xmmword ptr [rdx + 208]
	movaps  xmm15, xmmword ptr [rdx + 224]

	mov     rsp, qword ptr [rdx + 8]
	push    qword ptr [rdx + 0]

	ret

swap_context ENDP

ini_context PROC
	ret
ini_context ENDP

END
