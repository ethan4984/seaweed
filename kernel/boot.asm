videomode equ 280
kernelEntry equ 0x100000

pml4 equ 0x1000
pml3 equ 0x2000
pml2 equ 0x3000
hh_pml2 equ 0x4000
hh_pml3 equ 0x5000
e820Entries equ 0x6000

bits 16
org 0x7c00

cld
jmp 0:initCS

initCS:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov sp, 0x7c00

mov byte [BootDrive], dl

in al, 0x92
or al, 2
out 0x92, al ; enables a20 line

lgdt[GDT]

mov ah, 0x42 ; extended sector read from disk
mov si, DAP
int 0x13

xor ax, ax
mov di, pml4
mov cx, 0x6000 / 2
rep stosw ; zeros out 0x1000 -> 0x6000

xor ebx, ebx
mov edi, e820Entries

e820:
    mov eax, 0xe820
    mov ecx, 24 ; size of struct 
    mov edx, 0x534d4150 ; 'SMAP'
    int 0x15
    
    add edi, 24

    jc .exit ; error on carry

    mov al, byte [e820EntryNumber]
    inc al
    mov byte [e820EntryNumber], al
  
    test ebx, ebx ; when is zero we are done
    jnz e820

.exit:

jmp 0x8000

times 218-($-$$) db 0
times 6 db 0

BootDrive: db 0

DAP:
    .size: db 0x10 ; size of DAP
    .unused: db 0 ; unused
    .numberOfSectors: dw 63 ; number of sectors to be read
    .offset: dd 0x7e00 ; load point at
    .lbaStart: dq 1 ; first sector
    
GDT:
    dw .end - .start - 1 
    dd .start

.start:

.NULL:
    dq 0
.CODE16:
    dw 0xffff ; limit
    dw 0 ; base low
    db 0 ; base mid
    db 0b10011010 ; access
    db 0 ; granularity
    db 0 ; base high
.DATA16:
    dw 0xffff ; limit
    dw 0 ; base low
    db 0 ; base mid
    db 0b10011010 ; access
    db 0 ; granularity
    db 0 ; base high
.CODE32:
    dw 0xffff ; limit
    dw 0 ; base low
    db 0 ; base mid
    db 0b10011010 ; access
    db 0b11001111 ; granularity
    db 0 ; base high
.DATA32:
    dw 0xffff ; limit
    dw 0 ; base low
    db 0 ; base mid
    db 0b10010010 ; access
    db 0b11001111 ; granularity
    db 0 ; base high
.CODE64:
    dw 0 ; limit
    dw 0 ; base low
    db 0 ; base mid 
    db 0b10011010 ; access
    db 0b00100000 ; granularity
    db 0 ; base high
.DATA64:
    dw 0 ; limit
    dw 0 ; base low
    db 0 ; base mid
    db 0b10010010 ; access
    db 0 ; granularity
    db 0 ; base high
.USERCODE64:
    dw 0 ; limit
    dw 0 ; base low
    db 0 ; base mid
    db 0b11111101 ; access
    db 0b10101111 ; granularity
    db 0 ; base high
.USERDATA64:
    dw 0 ; limit
    dw 0 ; base low
    db 0 ; base mid
    db 0b11110011 ; access
    db 0b11001111 ; granularity
    db 0 ; base high

.end:

e820EntryNumber: db 0

times 0x1b8-($-$$) db 0 ; partition entry structures (left blanck dont touch)

times 510-($-$$) db 0
dw 0xaa55 ; boot signature

cld
jmp 0:initCSTramp

initCSTramp:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov sp, 0x7c00

in al, 0x92
or al, 2
out 0x92, al ; enables a20 line

lgdt[GDT]

mov eax, dword [0x500 + 8]
mov cr3, eax

mov eax, cr4
or eax, (1 << 5) | (1 << 7) ; set PAE and PGE
mov cr4, eax

mov ecx, 0xc0000080
rdmsr
or eax, (1 << 8) ; LME enable
wrmsr

mov eax, cr0
or eax, (1 << 31) | (1 << 0)
mov cr0, eax

jmp GDT.CODE64 - GDT.start:smpLongModeCode

bits 64

smpLongModeCode:
    mov ax, GDT.DATA64 - GDT.start
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rsp, qword [0x500] ; stack
    mov rbx, qword [0x500 + 16] ; entry point
    mov rcx, qword [0x500 + 24] ; idt
    mov rax, qword [0x500 + 32] ; core number

    mov fs, ax

    lidt [rcx]

    jmp rbx

bits 16

times 1024-($-$$) db 0

mov word [DAP.numberOfSectors], 0x7e
mov dword [DAP.offset], 0xfc00
mov dword [DAP.lbaStart], 64

xor eax, eax
xor edx, edx

mov dl, byte [BootDrive]
mov ah, 0x42
mov si, DAP
int 0x13

mov eax, 0x4f01
mov ecx, videomode
mov edi, VBEINFO 
int 0x10

mov eax, 0x4f02
mov ebx, videomode
int 0x10

mov word [bootheader.mmapaddress], 0x6000

mov al, byte [e820EntryNumber]
mov byte [bootheader.mmapentries], al

mov ax, word [VBEINFO.pitch]
mov word [bootheader.pitch], ax

mov ax, word [VBEINFO.width]
mov word [bootheader.width], ax

mov ax, word [VBEINFO.height]
mov word [bootheader.height], ax

mov al, byte [VBEINFO.bpp]
mov byte [bootheader.bpp], al

mov eax, dword [VBEINFO.framebuffer]
mov dword [bootheader.framebuffer], eax

cli

mov word [pml4 + 256 * 8], pml3 | 0x3 ; offset for 0xffff8... is 256 * 8
mov word [pml4], pml3 | 0x3
mov word [pml3], pml2 | 0x3

mov word [pml4 + 511 * 8], hh_pml3 | 0x3 ; offset for 0xfffffffff8... is 511 * 8 on pml4 and on pml3 its 510 * 8
mov word [hh_pml3 + 510 * 8], hh_pml2 | 0x3

xor eax, eax
xor edi, edi

buildPageTables:
    or eax, (1 << 7) | 0x3 ; set them as present and writable and also set the size bit making us use 2mb pages
    mov dword [pml2 + edi], eax  
    mov dword [hh_pml2 + edi], eax

    add eax, 0x200000
    add edi, 8

    cmp eax, 0x40000000 ; map first gb
    jb buildPageTables

mov eax, pml4
mov cr3, eax

mov eax, cr4
or eax, (1 << 5) | (1 << 7) ; set PAE and PGE set PSE
mov cr4, eax

mov ecx, 0xc0000080
rdmsr
or eax, 1 << 8 ; set LME in ELER
wrmsr

mov eax, cr0
or eax, 1 << 31 | 1 << 0 ; enable protected mode and paging
mov cr0, eax

jmp GDT.CODE64 - GDT.start:longModeCode ; far jump to cs code64

bits 64

longModeCode:
    mov ax, GDT.DATA64 - GDT.start
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    xor ax, ax
    mov fs, ax

    call findRSDP

    mov rdi, 0xffffffff80100000
    mov rsi, 0xfc00
    mov rcx, 0x10000 ; 64k
    rep movsb

    mov rdi, bootheader + 0xffff800000000000
    mov rsp, 0xffffffff80007c00
    mov rbp, 0xffffffff80007c00
    jmp 0xffffffff80100000

findRSDP:
    mov eax, 0x80000

.l1:
    cmp eax, 0xa0000 ; skip over video memory
    jne .continue
    mov eax, 0xe0000
    jmp .l1

.continue:
    xor ecx, ecx

.l2:
    mov dl, byte [rsdpSignature + ecx]
    cmp byte [eax + ecx], dl
    jne .signaturefail

    cmp ecx, 7
    je .signaturesuccess
    inc ecx

    jmp .l2

; r8 = 1 for equal
; r8 = 0 for not equal

.signaturesuccess:
    mov r8, 1
    jmp .endl2

.signaturefail:
    xor r8, r8
    jmp .endl2

.endl2:
    test r8, r8 
    jz .fail

    mov dword [bootheader.rsdp], eax
    ret

.fail:
    cmp eax, 0x100000
    je .end

    add eax, 16
    jmp .l1

.end:
    ret
    
rsdpSignature: db 'RSD PTR ',  0

bootheader:
    .mmapaddress: dq 0
    .mmapentries: db 0
    .rsdp: dd 0
    .pitch: dw 0
    .width: dw 0
    .height: dw 0
    .bpp: db 0
    .framebuffer: dd 0

VBEINFO:
    times 16 db 0
    .pitch: dw 0
    .width: dw 0
    .height: dw 0
    times 3 db 0
    .bpp: db 0
    times 14 db 0
    .framebuffer: dd 0
    times 212 db 0

times 32768-($-$$) db 0 ; mbrs limit

incbin 'Bin/kernel.bin'

times 131072-($-$$) db 0
