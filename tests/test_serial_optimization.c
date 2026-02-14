#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define SERIAL_PORT 0x3F8

// Mock state
int outb_count = 0;
int inb_count = 0;
int fifo_count = 0;
int fifo_capacity = 16;
bool fifo_empty = true;

// Mock implementations
void outb(uint16_t port, uint8_t val) {
    if (port == SERIAL_PORT) {
        outb_count++;
        fifo_count++;
        if (fifo_count > 0) {
            fifo_empty = false;
        }
    }
    // Simulate UART behavior: if FIFO is full, subsequent writes might overwrite or be ignored
    // For this test, we just track counts.
}

uint8_t inb(uint16_t port) {
    if (port == SERIAL_PORT + 5) {
        inb_count++;
        // Bit 5 (0x20) is THRE (Transmitter Holding Register Empty)
        // In FIFO mode, 1 means FIFO is empty. 0 means at least 1 byte in FIFO.
        // Wait, strictly speaking THRE=1 means FIFO is empty?
        // "In the FIFO mode, this bit is set to a '1' when the XMIT FIFO is empty,
        // and it is cleared to a '0' when at least 1 byte is written to the XMIT FIFO."

        // However, standard driver behavior is often:
        // if (LSR & 0x20) -> we can write.
        // But if FIFO is enabled, we can write up to 16 bytes.
        // But the bit only says "Empty" (1) or "Not Empty" (0).

        // If we write 1 byte, it becomes 0.
        // If we want to burst, we check if it is 1 (Empty), then write 16 bytes blindly.

        if (fifo_count == 0) {
            return 0x20;
        } else {
            return 0x00;
        }
    }
    return 0;
}

int is_transmit_empty() {
    return inb(SERIAL_PORT + 5) & 0x20;
}

// Optimized implementation to test
void serial_print_optimized(const char *str) {
    while (*str) {
        // Wait for FIFO to be empty
        while (is_transmit_empty() == 0) {
             // In a real test we need to simulate draining otherwise this hangs
             // checking endlessly.
             // Simulate drain for test purposes:
             if (fifo_count > 0) fifo_count--;
        }

        // Burst write up to 16 chars
        for (int i = 0; i < 16 && *str; ++i, ++str) {
            outb(SERIAL_PORT, *str);
        }
    }
}

// Original implementation for comparison (logic)
void serial_write_char_orig(char a) {
    while (is_transmit_empty() == 0) {
         if (fifo_count > 0) fifo_count--;
    }
    outb(SERIAL_PORT, a);
}

void serial_print_orig(const char *str) {
    for (const char *p = str; *p; ++p) {
        serial_write_char_orig(*p);
    }
}

void reset_mocks() {
    outb_count = 0;
    inb_count = 0;
    fifo_count = 0;
    fifo_empty = true;
}

int main() {
    const char *test_str = "This is a string that is definitely longer than 16 characters.";
    // length is 62 chars

    // Test Original
    reset_mocks();
    printf("Testing Original...\n");
    serial_print_orig(test_str);
    printf("Original: Writes=%d, StatusChecks=%d\n", outb_count, inb_count);

    // In original:
    // For each char:
    //   Check status (returns Empty, count=1)
    //   Write char (FIFO=1, Not Empty)
    //   Next loop: Check status (returns Not Empty, count=2)
    //   (Simulation decreases FIFO count) -> Returns Empty next time

    // Wait, my simulation of drain inside the check loop is a bit hacky but sufficient to show the difference.

    int orig_checks = inb_count;

    // Test Optimized
    reset_mocks();
    printf("Testing Optimized...\n");
    serial_print_optimized(test_str);
    printf("Optimized: Writes=%d, StatusChecks=%d\n", outb_count, inb_count);

    int opt_checks = inb_count;

    if (opt_checks < orig_checks) {
        printf("SUCCESS: Optimization reduced status checks from %d to %d\n", orig_checks, opt_checks);
    } else {
        printf("FAILURE: Optimization did not reduce status checks.\n");
        return 1;
    }

    // Verify correctness: Total writes should match string length
    // 62 chars
    if (outb_count != 62) {
        printf("FAILURE: Incorrect number of writes. Expected 62, got %d\n", outb_count);
        return 1;
    }

    return 0;
}
