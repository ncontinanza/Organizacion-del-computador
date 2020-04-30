// size_t copyfd(int fdin, int fdout, void *buf, size_t size);
.globl copyfd
copyfd:

    push     %r12       # Store r12-r15 values
    push     %r13
    push     %r14
    push     %r15
    push     %rbx
    push     %rbp

    movq     %rdi, %r12  # Save fdin in %r12
    movq     %rsi, %r13  # Save fdout in %r13
    movq     %rdx, %r14  # Save original *buf in %r14.
    movq     $0, %r15    # Bytes written

    # Read from fdin:
    # %rdi == fdin, so I'll leave it there.
    movq     %rdx, %rsi  # %rsi = *buf
    movq     %rcx, %rdx  # %rdx = size
LOOP:
    call     read        # read(fdin, *buf, size)

    # Check read result:
    cmp     $0, %rax
    jl      FAIL
    je      END_OK

    # Call to writeall wrapper:
WRITE:
    movq    %r13, %rdi
    movq    %r14, %rsi    # %rsi = original *buf
    movq    %rax, %rdx   # %rdx = bytes read
    call    writeall
    cmp     $0, %rax
    movq    %r12, %rdi
    movq    %r14, %rsi   # %rsi = buf
    addq    %rax, %r15   # Add writeall return value to r15
    je      LOOP

    # End copyfd:
FAIL:
    movq    %r15, %rax   # Return bytes written
    JMP     RESTORE_STACK_AND_RET

END_OK:
    movq    $0, %rax

RESTORE_STACK_AND_RET:
    pop     %rbp
    pop     %rbx
    pop     %r15         # Restore r12-r15 registers before ret
    pop     %r14
    pop     %r13
    pop     %r12

    ret
