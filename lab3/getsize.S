// ssize_t getsize(int atdirfd, const char *filename);
.globl getsize
getsize:

    subq    $144, %rsp      # Make room for stat

    # Prepare registers to call fstatat(int dirfd, const char *pathname, struct stat *buf, int flags)
    # %rdi == atdirfd, so I'll leave it there.
    # The same with %rsi, as %rsi == filename
    movq    %rsp, %rdx      # rdx <- *buf
    movq    $0, %rcx        # rcx <- flags (set them to 0)
    call    fstatat
    cmp     $0, %rax
    movq    %rdx, %rax            # %rax <- *buf
    jne     FAIL               
    leaq    24(%rax), %rax  # Move buf to st_mode position
    movl    (%rax), %edi    # Get st_mode
    # movq 24(%rax), %rdi     # Move buf to st_mode position
    andq    $0x8000, %rdi   # Use 0x8000 as a mask to check if file is regular
    cmp     $0, %rdi
    je      NOT_REGULAR
    addq    $24, %rax       # Mov buf to st_size
    movq    (%rax), %rax    # Set the size value

END:
    addq    $144, %rsp # Restore %rsp
    ret

FAIL:
    movq    $0, %rax        # Set return value to 0 if it fails
    jmp     END

NOT_REGULAR:
    movq    $-1, %rax       # Set return value to -1 if file is not regular
    jmp     END
