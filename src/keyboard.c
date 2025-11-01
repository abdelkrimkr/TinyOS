
#include "keyboard.h"
#include "ports.h"
#include "idt.h"

#define KBD_DATA_PORT 0x60

static char key_buffer[256];
static int read_idx = 0;
static int write_idx = 0;

// Scancode to ASCII map for US keyboard layout
static const char scancode_map[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0,
    ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static void keyboard_handler(registers_t regs) {
    (void)regs;
    uint8_t scancode = inb(KBD_DATA_PORT);
    if (scancode < sizeof(scancode_map) && scancode_map[scancode] != 0) {
        if (scancode != 0x0E) { // Don't buffer backspace for now
            key_buffer[write_idx++] = scancode_map[scancode];
            if (write_idx == 256) write_idx = 0;
        }
    }
}

void keyboard_init() {
    register_interrupt_handler(33, keyboard_handler);
}

char keyboard_getchar() {
    if (read_idx == write_idx) {
        return 0; // Buffer is empty
    }
    char c = key_buffer[read_idx++];
    if (read_idx == 256) read_idx = 0;
    return c;
}
