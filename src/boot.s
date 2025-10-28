section .multiboot
align 4
    dd 0x1BADB002            ; magic number (Multiboot)
    dd 0x00                  ; flags (no memory info)
    dd -(0x1BADB002 + 0x00)  ; checksum

section .text
global _start
extern kmain

_start:
    ; set up stack (simple, static)
    mov esp, stack_top

    ; call our C kernel
    call kmain

.hang:
    cli
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384 ; 16 KB stack
stack_top:
