.globl main

main:
	push $msg
	call strlen
	push %eax
	push $msg
        push $1
	
	push $posicion_retorno
        // No declaration needed; asm assumes symbols always exist.
        jmp write

posicion_retorno:
	pop %ebx
	pop %ebx
	pop %ebx
	pop %ebx
	pop %ebx
        push $7
        call _exit

.data
msg:
        .ascii "Hello, world!\n"

