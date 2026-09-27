.text
.globl swap_context
.globl _swap_context
#if defined(_WIN32)
.def _swap_context; .scl 2; .type 32; .endef
.def swap_context; .scl 2; .type 32; .endef
#endif
_swap_context:
swap_context:
	movl	4(%esp), %eax   /* old_context */
	movl	8(%esp), %edx   /* new_context */

	movl	%ebx, 0(%eax)
	movl	%ebp, 4(%eax)
	movl	%esi, 8(%eax)
	movl	%edi, 12(%eax)

	leal	4(%esp), %ecx
	movl	%ecx, 16(%eax)

	movl	(%esp), %ecx
	movl	%ecx, 20(%eax)

	movl	0(%edx), %ebx
	movl	4(%edx), %ebp
	movl	8(%edx), %esi
	movl	12(%edx), %edi

	movl	16(%edx), %esp
	pushl	20(%edx)

	ret

.globl ini_context
.globl _ini_context
#if defined(_WIN32)
.def _ini_context; .scl 2; .type 32; .endef
.def ini_context; .scl 2; .type 32; .endef
#endif
_ini_context:
ini_context:
	ret
