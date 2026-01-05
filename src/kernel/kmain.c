#include <stdint.h>

#define SERIAL_PORT 0x3f8
#define C_TEAL  "\033[36m"
#define C_GREEN "\033[32m"
#define C_RESET "\033[0m"

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "d"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

void serial_putc(char c) {
    while ((inb(SERIAL_PORT + 5) & 0x20) == 0);
    outb(SERIAL_PORT, c);
}

void log_msg(const char* color, const char* msg) {
    const char* p;
    for (p = color; *p; ++p) serial_putc(*p);
    for (p = msg; *p; ++p) serial_putc(*p);
    for (p = C_RESET; *p; ++p) serial_putc(*p);
}

void kmain(uint64_t mb_info) {
    // Init serial: disable ints, set baud divisor 3 (38400), 8N1
    outb(SERIAL_PORT + 1, 0x00);
    outb(SERIAL_PORT + 3, 0x80);
    outb(SERIAL_PORT, 0x03);
    outb(SERIAL_PORT + 1, 0x00);
    outb(SERIAL_PORT + 3, 0x03);

    log_msg(C_TEAL, "\n\n🎨 Palette OS Initializing...\n");

    uint8_t* tag = (uint8_t*)(mb_info + 8);
    uint32_t total_size = *(uint32_t*)mb_info;

    while (tag < (uint8_t*)(mb_info + total_size)) {
        uint32_t type = *(uint32_t*)tag;
        uint32_t size = *(uint32_t*)(tag + 4);
        if (type == 0) break;

        if (type == 5) { // Framebuffer
            log_msg(C_GREEN, "✓ Framebuffer found. Painting UI.\n");
            uint64_t addr = *(uint64_t*)(tag + 8);
            uint32_t pitch = *(uint32_t*)(tag + 16);
            uint32_t width = *(uint32_t*)(tag + 20);
            uint32_t height = *(uint32_t*)(tag + 24);
            uint8_t bpp = *(uint8_t*)(tag + 28);

            if (bpp == 32) {
                for (uint32_t y = 0; y < height; y++) {
                    for (uint32_t x = 0; x < width; x++) {
                        // Color: 0xFF008080 (Teal)
                        *(uint32_t*)(addr + y * pitch + x * 4) = 0xFF008080;
                    }
                }
            }
        }
        tag += ((size + 7) & ~7);
    }

    log_msg(C_TEAL, "✓ Boot complete. Enjoy the view!\n");
    while (1) asm volatile("hlt");
}
