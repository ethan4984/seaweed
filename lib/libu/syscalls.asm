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


