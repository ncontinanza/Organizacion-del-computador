# size_t writeall(int fd, const void *buf, size_t count);
# %rdi = fd , %rsi = *buf , %rdx = count

# En %rax va el numero de de sys_write que es 1
# En %rbx va el file descriptor
# En %rcx va el buffer
# En %rdx va la cantidad de bytes a escribir

# Cuando termina, en %rax queda la cantidad de bytes escritos realmente
.globl writeall
writeall:
    # Guardo el total en otro registro, asi en caso de que falle, puedo devolver la cantidad copiada
    movq %rdx, %r8

    # Cargo los arumentos en los registros correspondientes
LOOP:
    movq $1, %rax   # rax = 1 , que es sys_write
    movq %rdi, %rbx # rbx = rdi , en rdi esta el file descriptor al cual tengo que escribir
    movq %rsi, %rcx # rcx = rs1 , en rsi esta el puntero al buffer donde estan mis datos
    movq %rdx, %r8  # rdx = r8 , en r8 tengo la cantidad de bytes a escribir
    syscall
    # Veo si devolvio error la sys call
    cmpq $0, %rax
    jl   ERROR
    # Resto la cantida de bytes escritos a r8, para calcular los que me quedan por escribir
    subq %rax, %r8
    cmpq $0, %r8
    # Si r8 es mayor a 0, vuelvo al loop
    jg   LOOP
    # Cuando me voy por aca es porque termino bien, entonces copio la cantidad de bytes escritos a
    # rax, que van a ser la cantidad que esta en rdx, ya que todo termino bien
    movq $0, %rax
    jmp  DONE

ERROR:
    subq %rdx, %r8
    movq %r8, %rax

DONE:
    ret
