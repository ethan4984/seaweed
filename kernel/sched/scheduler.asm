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
