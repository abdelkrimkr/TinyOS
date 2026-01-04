#include <stdint.h>

#define SERIAL_PORT 0x3F8

// Colors
#define GREEN "\033[32m"
#define RESET "\033[0m"
#define CYAN "\033[36m"
#define GRAY "\033[90m"

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "d"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

void serial_init() {
    outb(SERIAL_PORT + 1, 0x00);    // Disable all interrupts
    outb(SERIAL_PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(SERIAL_PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(SERIAL_PORT + 1, 0x00);    //                  (hi byte)
    outb(SERIAL_PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(SERIAL_PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(SERIAL_PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int is_transmit_empty() {
    return inb(SERIAL_PORT + 5) & 0x20;
}

void serial_putc(char c) {
    while (is_transmit_empty() == 0);
    outb(SERIAL_PORT, c);
}

void serial_puts(const char* str) {
    while (*str) serial_putc(*str++);
}

void kmain(uint64_t multiboot_info_addr) {
    (void)multiboot_info_addr; // Suppress unused warning

    serial_init();

    serial_puts("\n");
    serial_puts(CYAN    "================================================\n" RESET);
    serial_puts(CYAN    "               PALETTE OS KERNEL                \n" RESET);
    serial_puts(CYAN    "================================================\n" RESET);
    serial_puts(GRAY    "Initializing system components...\n" RESET);
    serial_puts(GREEN   "[OK] " RESET "Serial Port Initialized\n");
    serial_puts(GREEN   "[OK] " RESET "Kernel Loaded Successfully\n");
    serial_puts("\n");

    while (1) {
        asm volatile("hlt");
    }
}
