%include 'lib/asmMacros.inc'

global startTask

startTask:
    mov rsp, rdi
    mov rbp, rdi
    sti
    jmp rsi

global switchTask

switchTask:
    mov rsp, rdi
    mov rbp, rdi

    popall
    add rsp, 16 
    sti
    iretq

global spinLock

extern ksleep

spinLock:
    lock bts dword [rdi], 0
    jc .l1
    ret

.l1:
    test dword [rdi], 1
    jnz .l1
    jmp spinLock
