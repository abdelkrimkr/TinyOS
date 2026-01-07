#include <stdint.h>

#define SERIAL_PORT 0x3F8

// ANSI Color Codes
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"
#define COLOR_BOLD    "\033[1m"

// I/O Port wrappers
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "d"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

// Serial functions
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

void serial_write_char(char a) {
    while (is_transmit_empty() == 0);
    outb(SERIAL_PORT, a);
}

void serial_print(const char *str) {
    for (const char *p = str; *p; ++p) {
        serial_write_char(*p);
    }
}

void log_ok(const char *msg) {
    serial_print("[  ");
    serial_print(COLOR_GREEN);
    serial_print("OK");
    serial_print(COLOR_RESET);
    serial_print("  ] ");
    serial_print(msg);
    serial_print("\n");
}

void log_info(const char *msg) {
    serial_print("[ ");
    serial_print(COLOR_CYAN);
    serial_print("INFO");
    serial_print(COLOR_RESET);
    serial_print(" ] ");
    serial_print(msg);
    serial_print("\n");
}

void kmain(uint64_t multiboot_info_addr) {
    (void)multiboot_info_addr; // Suppress unused parameter warning

    serial_init();

    // Clear screen (ANSI)
    serial_print("\033[2J\033[1;1H");

    serial_print("\n");
    serial_print(COLOR_BOLD COLOR_CYAN "========================================" COLOR_RESET "\n");
    serial_print(COLOR_BOLD COLOR_CYAN "       KERNEL BOOTLOADER v0.1.0         " COLOR_RESET "\n");
    serial_print(COLOR_BOLD COLOR_CYAN "========================================" COLOR_RESET "\n");
    serial_print("\n");

    log_ok("Serial port initialized.");
    log_info("Booting kernel...");

    // Real work done
    log_ok("Kernel loaded successfully.");
    log_info("System halted.");
    serial_print(COLOR_YELLOW "Hint: Press Ctrl+A, then X to exit QEMU." COLOR_RESET "\n");

    while (1) {
        asm volatile("hlt");
    }
}
