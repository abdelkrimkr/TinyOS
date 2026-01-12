#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Mock Hardware State
#define FIFO_SIZE 16
static int fifo_count = 0;
static uint64_t inb_calls = 0;
static uint64_t outb_calls = 0;

// Mock I/O functions
uint8_t mock_inb(uint16_t port) {
    inb_calls++;
    if (port == 0x3F8 + 5) { // LSR
        // Bit 5 is THRE (Transmitter Holding Register Empty)
        // In FIFO mode: 1 if FIFO is empty, 0 if not empty.
        return (fifo_count == 0) ? 0x20 : 0x00;
    }
    return 0;
}

void mock_outb(uint16_t port, uint8_t val) {
    outb_calls++;
    if (port == 0x3F8) { // THR
        if (fifo_count < FIFO_SIZE) {
            fifo_count++;
        } else {
            // Overrun simulation (should not happen if logic is correct)
            printf("ERROR: FIFO Overrun!\n");
        }
    }
}

// Emulate hardware draining the FIFO
void tick_hardware() {
    if (fifo_count > 0) {
        fifo_count--;
    }
}

// --- Original Implementation ---
int is_transmit_empty_orig() {
    return mock_inb(0x3F8 + 5) & 0x20;
}

void serial_write_char_orig(char a) {
    while (is_transmit_empty_orig() == 0) {
        tick_hardware(); // Simulate time passing while waiting
    }
    mock_outb(0x3F8, a);
}

void serial_print_orig(const char *str) {
    for (const char *p = str; *p; ++p) {
        serial_write_char_orig(*p);
    }
}

// --- Optimized Implementation ---
int is_transmit_empty_opt() {
    return mock_inb(0x3F8 + 5) & 0x20;
}

void serial_print_opt(const char *str) {
    while (*str) {
        // Wait for FIFO to be empty
        while (is_transmit_empty_opt() == 0) {
            tick_hardware();
        }

        // Burst write up to 16 bytes
        for (int i = 0; i < 16 && *str; i++) {
            mock_outb(0x3F8, *str++);
        }
    }
}

int main() {
    const char *test_str = "Hello, this is a test string to measure UART performance. It needs to be long enough to fill the FIFO multiple times.";

    // Test Original
    fifo_count = 0;
    inb_calls = 0;
    outb_calls = 0;

    printf("Benchmarking Original...\n");
    serial_print_orig(test_str);
    printf("Original: inb_calls=%lu, outb_calls=%lu\n", inb_calls, outb_calls);
    uint64_t orig_inb = inb_calls;

    // Test Optimized
    fifo_count = 0;
    inb_calls = 0;
    outb_calls = 0;

    printf("Benchmarking Optimized...\n");
    serial_print_opt(test_str);
    printf("Optimized: inb_calls=%lu, outb_calls=%lu\n", inb_calls, outb_calls);

    if (inb_calls < orig_inb) {
        printf("SUCCESS: Reduced inb calls by %lu (%.2f%%)\n",
            orig_inb - inb_calls,
            100.0 * (orig_inb - inb_calls) / orig_inb);
    } else {
        printf("FAILURE: No improvement.\n");
    }

    return 0;
}
