	.file	"cp57.c"
	.text
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"Error en la cantidad de argumentos: %m\n"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC1:
	.string	"Error al abrir %s: %m\n"
.LC2:
	.string	"Error: %s es un directorio\n"
	.section	.rodata.str1.8
	.align 8
.LC3:
	.string	"Hubo un error leyendo el archivo %s\n"
	.align 8
.LC4:
	.string	"Finaliz\303\263 la copia del archivo exitosamente\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB16:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$536, %rsp
	.cfi_def_cfa_offset 592
	movq	%fs:40, %rax
	movq	%rax, 520(%rsp)
	xorl	%eax, %eax
	cmpl	$3, %edi
	je	.L2
	leaq	.LC0(%rip), %rdx
	movl	$1, %esi
	movq	stderr(%rip), %rdi
	call	__fprintf_chk@PLT
	movl	$1, %eax
.L1:
	movq	520(%rsp), %rcx
	xorq	%fs:40, %rcx
	jne	.L16
	addq	$536, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
.L2:
	.cfi_restore_state
	movq	%rsi, %r14
	movq	8(%rsi), %rdi
	movl	$0, %esi
	movl	$0, %eax
	call	open@PLT
	movl	%eax, %r15d
	testl	%eax, %eax
	js	.L17
	movq	16(%r14), %rdi
	movl	$438, %edx
	movl	$577, %esi
	movl	$0, %eax
	call	open@PLT
	movl	%eax, %r13d
	testl	%eax, %eax
	js	.L18
.L5:
	movq	%rsp, %rsi
	movl	$512, %edx
	movl	%r15d, %edi
	movl	$0, %eax
	call	read@PLT
	movslq	%eax, %rbx
	call	__errno_location@PLT
	movq	%rax, %r12
	cmpl	$4, (%rax)
	je	.L19
.L6:
	cmpl	$21, (%r12)
	je	.L20
	testq	%rbx, %rbx
	js	.L21
	testq	%rbx, %rbx
	je	.L22
	movl	$0, %ebp
	jmp	.L9
.L17:
	movq	8(%r14), %rcx
	leaq	.LC1(%rip), %rdx
	movl	$1, %esi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	__fprintf_chk@PLT
	movl	$1, %eax
	jmp	.L1
.L18:
	movq	16(%r14), %rcx
	leaq	.LC1(%rip), %rdx
	movl	$1, %esi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	__fprintf_chk@PLT
	movl	$1, %eax
	jmp	.L1
.L19:
	movq	%rsp, %rsi
	movl	$512, %edx
	movl	%r15d, %edi
	movl	$0, %eax
	call	read@PLT
	movslq	%eax, %rbx
	jmp	.L6
.L20:
	movq	8(%r14), %rcx
	leaq	.LC2(%rip), %rdx
	movl	$1, %esi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	__fprintf_chk@PLT
	movl	$1, %eax
	jmp	.L1
.L21:
	movq	8(%r14), %rcx
	leaq	.LC3(%rip), %rdx
	movl	$1, %esi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	__fprintf_chk@PLT
	movl	$1, %eax
	jmp	.L1
.L22:
	leaq	.LC4(%rip), %rdx
	movl	$1, %esi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	__fprintf_chk@PLT
	movl	$0, %eax
	jmp	.L1
.L10:
	cmpl	$21, (%r12)
	je	.L23
	testq	%rax, %rax
	js	.L24
	addq	%rax, %rbp
	cmpq	%rbp, %rbx
	je	.L5
.L9:
	movq	%rsp, %rsi
	movq	%rbx, %rdx
	movl	%r13d, %edi
	movl	$0, %eax
	call	write@PLT
	cltq
	cmpl	$4, (%r12)
	jne	.L10
	movq	%rsp, %rsi
	movq	%rbx, %rdx
	movl	%r13d, %edi
	movl	$0, %eax
	call	write@PLT
	cltq
	jmp	.L10
.L23:
	movq	16(%r14), %rcx
	leaq	.LC2(%rip), %rdx
	movl	$1, %esi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	__fprintf_chk@PLT
	movl	$1, %eax
	jmp	.L1
.L24:
	movq	16(%r14), %rcx
	leaq	.LC3(%rip), %rdx
	movl	$1, %esi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	__fprintf_chk@PLT
	movl	$1, %eax
	jmp	.L1
.L16:
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE16:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",@progbits
