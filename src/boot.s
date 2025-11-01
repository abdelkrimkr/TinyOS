
section .multiboot_header
header_start:
    ; Magic number
    dd 0xE85250D6
    ; Architecture (0 for i386)
    dd 0
    ; Header length
    dd header_end - header_start
    ; Checksum
    dd -(0xE85250D6 + 0 + (header_end - header_start))

    ; Framebuffer tag
    dw 5
    dw 0
    dd 8
    dd 1024
    dd 768
    dd 32

    ; Terminator tag
    dw 0
    dw 0
    dd 8
header_end:

section .text
global _start
extern kmain

_start:
    ; Set up the stack
    mov esp, stack_top
    ; Grub multiboot2 magic in eax
    ; Pointer to boot info in ebx.
    push ebx
    call kmain
    ; Hang
.hang:
    cli
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384 ; 16 KB stack
stack_top:
