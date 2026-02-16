.global start
.extern kmain

.section .text
.code32
start:
    /* Point the stack to a valid area */
    mov $stack_top, %esp

    /* Check if we are booted via Multiboot2 (EAX should be 0x36d76289) */
    cmpl $0x36d76289, %eax
    jne no_multiboot

    /* Save Multiboot2 information structure pointer (EBX) */
    mov %ebx, %edi

    /* 1. Check for Long Mode support */
    call check_long_mode

    /* 2. Set up Paging (Identity map first 2MB) */
    call setup_paging

    /* 3. Enable PAE */
    mov %cr4, %eax
    or $(1 << 5), %eax
    mov %eax, %cr4

    /* 4. Load P4 to CR3 */
    mov $p4_table, %eax
    mov %eax, %cr3

    /* 5. Enable Long Mode (EFER MSR) */
    mov $0xC0000080, %ecx
    rdmsr
    or $(1 << 8), %eax
    wrmsr

    /* 6. Enable Paging */
    mov %cr0, %eax
    or $(1 << 31), %eax
    mov %eax, %cr0

    /* Load 64-bit GDT */
    lgdt gdt64_pointer

    /* Jump to 64-bit code segment */
    ljmp $0x8, $long_mode_start

no_multiboot:
    hlt

check_long_mode:
    /* Check CPUID support */
    pushf
    pop %eax
    mov %eax, %ecx
    xor $(1 << 21), %eax
    push %eax
    popf
    pushf
    pop %eax
    push %ecx
    popf
    cmp %ecx, %eax
    je no_long_mode

    /* Check Extended Function available */
    mov $0x80000000, %eax
    cpuid
    cmpl $0x80000001, %eax
    jb no_long_mode

    /* Check Long Mode availability */
    mov $0x80000001, %eax
    cpuid
    test $(1 << 29), %edx
    jz no_long_mode
    ret

no_long_mode:
    movb $'L', %al
    mov $0x3f8, %dx
    outb %al, %dx
    hlt

setup_paging:
    /* Map first P4 entry to P3 table */
    mov $p3_table, %eax
    or $0b11, %eax /* present + writable */
    mov %eax, p4_table

    /* Map first P3 entry to P2 table */
    mov $p2_table, %eax
    or $0b11, %eax /* present + writable */
    mov %eax, p3_table

    /* Map each P2 entry to a huge 2MB page */
    mov $0, %ecx         /* counter */
map_p2_table:
    /* Map 2MB page */
    mov $0x200000, %eax  /* 2MB */
    mul %ecx            /* start address of nth page */
    or $0b10000011, %eax /* present + writable + huge */
    mov %eax, p2_table(, %ecx, 8)

    inc %ecx
    cmpl $512, %ecx
    jne map_p2_table

    ret

.section .rodata
gdt64:
    .quad 0 /* Zero entry */
gdt64_code:
    .quad (1<<43) | (1<<44) | (1<<47) | (1<<53) /* Code segment */
gdt64_pointer:
    .word . - gdt64 - 1
    .quad gdt64

.section .text
.code64
long_mode_start:
    /* Clear segment registers */
    mov $0, %ax
    mov %ax, %ss
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    /* Jump to C kernel (pass multiboot info pointer in RDI) */
    /* EDI was saved earlier in 32-bit mode */
    /* In 64-bit System V ABI, first argument is in RDI. EDI is the lower 32-bits of RDI. */
    /* Since we are in 32-bit protected mode initially, the pointer fits in 32 bits. */
    /* We need to zero extend EDI to RDI just in case, but usually high bits are zeroed by 32-bit mov. */

    call kmain
    hlt

.section .bss
.align 4096
p4_table:
    .skip 4096
p3_table:
    .skip 4096
p2_table:
    .skip 4096
stack_bottom:
    .skip 4096 * 4
stack_top:

.section .note.GNU-stack,"",@progbits
