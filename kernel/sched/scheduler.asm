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
