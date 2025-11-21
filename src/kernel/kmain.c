#include <stdint.h>

void kmain(uint64_t multiboot_info_addr) {
    // Minimal verification: Write to serial port 0x3F8
    // COM1 is usually 0x3F8. We'll verify this by reading the output in QEMU if redirected.

    // Initialize serial port (minimal)
    // Disable interrupts
    asm volatile("outb %0, %1" : : "a"((uint8_t)0x00), "d"((uint16_t)0x3f8 + 1));
    // Set baud rate divisor
    asm volatile("outb %0, %1" : : "a"((uint8_t)0x80), "d"((uint16_t)0x3f8 + 3));
    asm volatile("outb %0, %1" : : "a"((uint8_t)0x03), "d"((uint16_t)0x3f8 + 0));
    asm volatile("outb %0, %1" : : "a"((uint8_t)0x00), "d"((uint16_t)0x3f8 + 1));
    // 8 bits, no parity, one stop bit
    asm volatile("outb %0, %1" : : "a"((uint8_t)0x03), "d"((uint16_t)0x3f8 + 3));

    const char *msg = "Kernel initialized successfully.\n";
    for (const char *p = msg; *p; ++p) {
        while (1) {
            uint8_t status;
            asm volatile("inb %1, %0" : "=a"(status) : "d"((uint16_t)0x3f8 + 5));
            if (status & 0x20) break; // Wait for transmit empty
        }
        asm volatile("outb %0, %1" : : "a"(*p), "d"((uint16_t)0x3f8));
    }

    while (1) {
        asm volatile("hlt");
    }
}
