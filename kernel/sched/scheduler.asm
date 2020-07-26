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
    iretq

global spinLock

spinLock:
    lock bts dword [rdi], 0
    jc .loop
    ret

.loop:
    pause
    test dword [rdi], 1
    jnz .loop
    jmp spinLock
