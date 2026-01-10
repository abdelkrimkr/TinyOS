#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// Mock Hardware State
static uint8_t registers[8];
static uint8_t fifo[16];
static int fifo_head = 0;
static int fifo_count = 0;

static int inb_calls = 0;
static int outb_calls = 0;

#define SERIAL_PORT 0x3F8

// Mock Implementation with Auto-Drain
void mock_outb(uint16_t port, uint8_t val) {
    outb_calls++;
    uint16_t offset = port - SERIAL_PORT;
    if (offset == 0) {
        // Data port - write to FIFO
        if (fifo_count >= 16) {
            printf("ERROR: FIFO Overflow!\n");
        } else {
            fifo[fifo_head] = val;
            fifo_head = (fifo_head + 1) % 16;
            fifo_count++;
        }

        // If FIFO is not empty, THRE (bit 5) is 0.
        // In FIFO mode, THRE=1 means FIFO is empty.
        if (fifo_count > 0) {
            registers[5] &= ~0x20;
        }
    } else if (offset == 5) {
        // Read-only
    } else {
        registers[offset] = val;
    }
}

uint8_t mock_inb(uint16_t port) {
    inb_calls++;
    if (inb_calls > 100000) {
        printf("ERROR: Infinite loop detected in serial_print!\n");
        exit(1);
    }

    uint16_t offset = port - SERIAL_PORT;
    if (offset == 5) {
        // Drain logic
        if (fifo_count > 0) {
            fifo_count--;
            if (fifo_count == 0) {
                registers[5] |= 0x20; // Set THRE (Empty)
            }
        }
        return registers[5];
    }
    return registers[offset];
}

// Redirect mocks BEFORE including kernel code
#define TEST_MODE
#define outb mock_outb
#define inb mock_inb

#include "../src/kernel/kmain.c"

int main() {
    printf("Running Serial Performance Test...\n");

    // Init mocks
    memset(registers, 0, sizeof(registers));
    registers[5] = 0x20; // THRE starts empty
    fifo_count = 0;
    inb_calls = 0;
    outb_calls = 0;

    const char *test_str = "0123456789ABCDEF0123456789ABCDEF"; // 32 chars

    // Run
    serial_print(test_str);

    printf("Stats: inb_calls=%d, outb_calls=%d\n", inb_calls, outb_calls);

    // EXPECTATION:
    // With optimization, we check status once for every 16 bytes.
    // 32 chars -> 2 bursts.
    // Burst 1:
    //   read status (empty) -> 1 inb call.
    //   write 16 chars -> 16 outb calls.
    // Burst 2:
    //   read status.
    //   If using mock logic, the "drain" happens during read.
    //   But mock_inb logic is: "drain 1 byte per call".
    //   Wait, if we wrote 16 bytes, fifo_count=16.
    //   We call inb. fifo_count becomes 15. Returns Not Empty (bit 5 cleared).
    //   Loop continues.
    //   We call inb. fifo_count becomes 14. Returns Not Empty.
    //   ...
    //   We call inb. fifo_count becomes 0. Returns Empty.
    //   Loop exits.
    //   Total inb calls to drain 16 bytes: 16 calls.
    //   Then write 16 bytes.

    // Total inb calls = 1 (initial) + 16 (wait for drain) = 17.
    // Total outb calls = 32.

    // Unoptimized was 32 inb calls because:
    //   Write 1 char. Fifo=1.
    //   Read status. Fifo=1 -> drain -> Fifo=0. Returns Empty.
    //   Wait, 1 read per char to clear buffer.

    // So:
    // Unoptimized: 32 writes, 32 reads (assuming instant drain per read).
    // Optimized: 32 writes, 17 reads (1 initial + 16 to drain the first burst).

    // Wait, 17 reads vs 32 reads.
    // Improvement: ~47% reduction in reads.
    // This confirms optimization works.

    if (outb_calls != 32) {
        printf("FAILED: Expected 32 writes, got %d\n", outb_calls);
        return 1;
    }

    // We expect FEWER reads than writes roughly, or at least fewer than unoptimized.
    if (inb_calls >= 32) {
        printf("FAILED: Expected fewer reads than baseline (32), got %d\n", inb_calls);
        return 1;
    }

    printf("Test passed (Optimized).\n");
    return 0;
}
