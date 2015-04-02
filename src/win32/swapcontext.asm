.model FLAT, C

.code

swap_context PROC

	; eax father_context dword ptr [esp+4]
	; ecx last_context dword ptr [esp+8]
	
	push	EAX
	mov		EAX,	dword ptr [esp+8]

	; save
	mov		dword ptr [eax+4],  EBX
	mov		dword ptr [eax+8],  ECX
	mov		dword ptr [eax+12], EDX
	mov		dword ptr [eax+16], ESI
	mov		dword ptr [eax+20], EDI
	
	;; ESP
	lea		ECX,	[esp] + 8
	mov		dword ptr [eax+24], ECX

	mov		dword ptr [eax+28], EBP

	;; EIP
	mov		ECX,	dword ptr [esp + 4]
	mov		dword ptr [eax+32], ECX
	
	mov		word ptr [eax+36], SS
	mov		word ptr [eax+40], ES
	mov		word ptr [eax+44], DS
	mov		word ptr [eax+48], FS
	
	mov		ECX,	EAX
	pop		EAX
	mov		dword ptr [ECX+0],  EAX

	mov		ECX,	dword ptr [esp+8]

	; load
	mov		EAX,  dword ptr [ecx+0]
	mov		EBX,  dword ptr [ecx+4]
	mov		EDX,  dword ptr [ecx+12]
	mov		ESI,  dword ptr [ecx+16]
	mov		EDI,  dword ptr [ecx+20]
	mov		ESP,  dword ptr [ecx+24]
	mov		EBP,  dword ptr [ecx+28]
	
	mov		FS,		word ptr [ecx+48]
	mov		ES,		word ptr [ecx+40]
	mov		DS,		word ptr [ecx+44]
	mov		SS,		word ptr [ecx+36]

	push	dword ptr [ecx+32]
	mov		ECX,  dword ptr [ecx+8]

	ret 

swap_context ENDP

ini_context PROC

	; ecx context

	mov		ECX,	dword ptr [esp+4]

	mov		word ptr [ecx+36], SS
	mov		word ptr [ecx+40], ES
	mov		word ptr [ecx+44], DS
	mov		word ptr [ecx+48], FS

	ret 

ini_context ENDP

get_rbp PROC

	mov		eax,	ebp

	ret 

get_rbp ENDP

end
