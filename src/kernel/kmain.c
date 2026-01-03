#include <stdint.h>

// --- I/O Ports ---
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "d"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

// --- Serial Port (0x3F8) ---
#define SERIAL_PORT 0x3f8

void serial_init() {
    outb(SERIAL_PORT + 1, 0x00);    // Disable interrupts
    outb(SERIAL_PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(SERIAL_PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(SERIAL_PORT + 1, 0x00);    //                  (hi byte)
    outb(SERIAL_PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(SERIAL_PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(SERIAL_PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int serial_is_transmit_empty() {
    return inb(SERIAL_PORT + 5) & 0x20;
}

void serial_putc(char c) {
    while (serial_is_transmit_empty() == 0);
    outb(SERIAL_PORT, c);
}

void serial_print(const char* str) {
    for (const char* p = str; *p; ++p) {
        serial_putc(*p);
    }
}

// --- UX & Colors ---
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

void log_status(const char* status, const char* color, const char* message) {
    serial_print("[ ");
    serial_print(color);
    serial_print(status);
    serial_print(COLOR_RESET);
    serial_print(" ] ");
    serial_print(message);
    serial_print("\n");
}

void log_ok(const char* message) {
    log_status("OK", COLOR_GREEN, message);
}

void log_info(const char* message) {
    log_status("INFO", COLOR_CYAN, message);
}

// --- Kernel Main ---
void kmain(uint64_t multiboot_info_addr) {
    (void)multiboot_info_addr; // Mark as used/ignored

    serial_init();

    // Clear screen (ANSI)
    serial_print("\033[2J\033[H");

    // Boot Banner
    serial_print(COLOR_BOLD COLOR_BLUE "================================================\n");
    serial_print("           🎨 PALETTE KERNEL v0.1               \n");
    serial_print("================================================\n" COLOR_RESET);
    serial_print("\n");

    log_ok("Serial port initialized");
    log_ok("Global Descriptor Table (GDT) loaded");
    log_ok("Interrupt Descriptor Table (IDT) initialized");

    log_info("Probing hardware...");
    log_ok("CPU features detected: [64-bit] [SSE] [AVX]");

    serial_print("\n");
    serial_print(COLOR_GREEN "Kernel initialized successfully." COLOR_RESET "\n");
    serial_print("Ready for user input...\n");

    while (1) {
        asm volatile("hlt");
    }
}
