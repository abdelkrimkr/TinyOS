#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Mock Hardware State
uint8_t mock_lsr = 0x20; // Initial state: THR Empty (Bit 5 set)
int mock_fifo_count = 0; // Bytes in FIFO
int inb_calls = 0;
int outb_calls = 0;

#define SERIAL_PORT 0x3F8

// Mock Functions
uint8_t inb(uint16_t port) {
    if (port == SERIAL_PORT + 5) { // LSR
        inb_calls++;
        // If FIFO has space, Bit 5 is 1?
        // Wait, strictly:
        // Bit 5 (THRE) = 1 if FIFO is EMPTY (0 bytes).
        // Bit 5 (THRE) = 0 if FIFO has Data (>= 1 byte).
        // (This is a simplified model of 16550A logic where we only check for EMPTY to start a burst)
        if (mock_fifo_count == 0) {
            return 0x20;
        } else {
            return 0x00;
        }
    }
    return 0;
}

void outb(uint16_t port, uint8_t val) {
    if (port == SERIAL_PORT) { // Data
        outb_calls++;
        mock_fifo_count++;
        // Simulate hardware drain happens "sometime", but for this test we manually drain or just fill.
    }
}

int is_transmit_empty() {
    return inb(SERIAL_PORT + 5) & 0x20;
}

// Baseline Implementation (from kmain.c)
void serial_write_char_baseline(char a) {
    while (is_transmit_empty() == 0) {
        // Simulate waiting: assume hardware drains 1 byte eventually
        if (mock_fifo_count > 0) mock_fifo_count--;
    }
    outb(SERIAL_PORT, a);
}

void serial_print_baseline(const char *str) {
    for (const char *p = str; *p; ++p) {
        serial_write_char_baseline(*p);
    }
}

// Optimized Implementation
void serial_print_optimized(const char *str) {
    while (*str) {
        // Wait for FIFO empty (Bit 5 == 1)
        while (is_transmit_empty() == 0) {
             if (mock_fifo_count > 0) mock_fifo_count--;
        }

        // Write up to 16 bytes
        for (int i = 0; i < 16 && *str; i++, str++) {
            outb(SERIAL_PORT, *str);
        }
    }
}

void reset_mocks() {
    mock_lsr = 0x20;
    mock_fifo_count = 0;
    inb_calls = 0;
    outb_calls = 0;
}

int main() {
    const char *test_str = "Hello, Kernel! This is a long string to test the burst mode of the serial port.";

    // Test Baseline
    reset_mocks();
    serial_print_baseline(test_str);
    printf("Baseline: %d chars -> %d inb calls (status checks)\n", (int)strlen(test_str), inb_calls);

    int baseline_calls = inb_calls;

    // Test Optimized
    reset_mocks();
    serial_print_optimized(test_str);
    printf("Optimized: %d chars -> %d inb calls (status checks)\n", (int)strlen(test_str), inb_calls);

    int optimized_calls = inb_calls;

    printf("Improvement: %.2fx fewer status checks\n", (float)baseline_calls / optimized_calls);

    return 0;
}
