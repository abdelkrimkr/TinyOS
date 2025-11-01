
section .text

global idt_load
extern isr_handler

idt_load:
    lidt [esp + 4]
    ret

isr_common_stub:
    pusha
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call isr_handler
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    add esp, 8
    iret

%macro ISR 2
global isr%1
isr%1:
    cli
    %2
    push %1
    jmp isr_common_stub
%endmacro

%macro IRQ 2
global irq%1
irq%1:
    cli
    push 0
    push %2
    jmp isr_common_stub
%endmacro

; ISRs
ISR 0, push 0
ISR 1, push 0
ISR 2, push 0
ISR 3, push 0
ISR 4, push 0
ISR 5, push 0
ISR 6, push 0
ISR 7, push 0
ISR 8, nop
ISR 9, push 0
ISR 10, nop
ISR 11, nop
ISR 12, nop
ISR 13, nop
ISR 14, nop
ISR 15, push 0
ISR 16, push 0
ISR 17, nop
ISR 18, push 0
ISR 19, push 0
ISR 20, push 0
ISR 21, nop
ISR 22, push 0
ISR 23, push 0
ISR 24, push 0
ISR 25, push 0
ISR 26, push 0
ISR 27, push 0
ISR 28, nop
ISR 29, nop
ISR 30, nop
ISR 31, nop

; IRQs
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47
