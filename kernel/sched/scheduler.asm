%include 'lib/asmMacros.inc'

global startTask

startTask:
    mov rsp, rdi
    sti
    jmp rsi

global switchTask

switchTask:
    mov rsp, rdi
    mov rbp, rsi

    popall
    sti
    add rsp, 16
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
