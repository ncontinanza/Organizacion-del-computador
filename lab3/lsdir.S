// void lsdir(DIR *dir);
# %rdi = *dir
# uso readdir para obtener el siguiente struct dirent
# y por cada struct, imprimo nombre o nombre y tamaño del archivo en cuestion
.globl lsdir
lsdir:
    movq  %rdi,%r15     # Me guardo el puntero *dir
    call  dirfd
    movq  %rax,%rbx     # Me guardo el file descriptor asociado a la carpeta en rbx
loop:
    movq  %r15, %rdi    # Pongo el puntero *dir de nuevo en rdi
    call  readdir
    cmp   $0, %rax
    jle   termine
    leaq  19(%rax),%rsi # Copio el puntero al filename a rsi para llamar a getsize
    leaq  19(%rax),%r14      # Me guardo el puntero al filename
    movq  %rbx, %rdi    # Copio el file descriptor asociado a la carpeta
    call  getsize
    movq  %rax, %rdx    # Copio el tamaño a rdx
    cmp   $0, %rax
    jge   regular      # 0 es un tamaño valido de archivo regular

not_regular:
    leaq  fmt_nreg, %rdi
    jmp   mostrar

regular:
    leaq  fmt_reg, %rdi

mostrar:
    movq  %r14, %rsi
    movb  $0, %al
    call  printf
    jmp   loop

termine:
    ret

.data
fmt_reg:
   .asciz "%s\t%zu\n"  // d_name, size
fmt_nreg:
   .asciz "%s\n"       // d_name
