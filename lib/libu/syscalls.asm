global printSerial

printSerial:
    push rax
    push rcx
    push rdx

    xor rax, rax
    xor rcx, rcx
    mov rdx, rdi
    int 0x69

    pop rdx
    pop rcx
    pop rax

    ret

pthread: 
    push rax
    push rcx
    push rdx
    push rbx

    mov rax, 2
    xor rcx, rcx
    mov rdx, rdi
    mov rbx, rsi
    int 0x69

    pop rbx
    pop rdx
    pop rcx
    pop rax
    
    ret
