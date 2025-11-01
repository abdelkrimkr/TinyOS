
#include "idt.h"
#include "ports.h"
#include <stddef.h>

#define IDT_ENTRIES 256
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idt_p;

isr_t interrupt_handlers[IDT_ENTRIES];

// Extern declarations for our interrupt handlers, defined in assembly
extern void idt_load(struct idt_ptr* idt_p);
extern void isr0(); extern void isr1(); extern void isr2(); extern void isr3(); extern void isr4(); extern void isr5();
extern void isr6(); extern void isr7(); extern void isr8(); extern void isr9(); extern void isr10(); extern void isr11();
extern void isr12(); extern void isr13(); extern void isr14(); extern void isr15(); extern void isr16(); extern void isr17();
extern void isr18(); extern void isr19(); extern void isr20(); extern void isr21(); extern void isr22(); extern void isr23();
extern void isr24(); extern void isr25(); extern void isr26(); extern void isr27(); extern void isr28(); extern void isr29();
extern void isr30(); extern void isr31();
extern void irq0(); extern void irq1(); extern void irq2(); extern void irq3(); extern void irq4(); extern void irq5();
extern void irq6(); extern void irq7(); extern void irq8(); extern void irq9(); extern void irq10(); extern void irq11();
extern void irq12(); extern void irq13(); extern void irq14(); extern void irq15();

void isr_handler(registers_t regs) {
    if (interrupt_handlers[regs.int_no] != 0) {
        interrupt_handlers[regs.int_no](regs);
    }

    // Send End-of-Interrupt (EOI) to the PICs
    if (regs.int_no >= 40) {
        outb(PIC2_COMMAND, 0x20);
    }
    outb(PIC1_COMMAND, 0x20);
}

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

static void pic_remap() {
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    outb(PIC1_DATA, 0x0);
    outb(PIC2_DATA, 0x0);
}

void idt_init() {
    idt_p.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idt_p.base = (uint32_t)&idt;

    for (int i=0; i<IDT_ENTRIES; i++) {
        interrupt_handlers[i] = NULL;
    }

    pic_remap();

    // Set up the ISRs
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
    // ... all other ISRs

    // Set up the IRQs
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
    // ... all other IRQs

    idt_load(&idt_p);
    asm volatile ("sti");
}
