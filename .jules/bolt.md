# Bolt's Journal

## 2025-02-14 - UART FIFO Performance Pattern
**Learning:** Standard UART drivers often implement `putc` by checking `is_transmit_empty()` before every byte. On 16550 UARTs, this bit often means "FIFO Empty", forcing the driver to wait for the FIFO to drain completely between every character, effectively serializing output to the baud rate and wasting CPU cycles on IO polling.
**Action:** Use a burst-write strategy: wait for FIFO empty, then write up to 16 bytes (FIFO size) in a tight loop without checking status. This leverages the hardware buffer and reduces expensive `inb` calls by ~50% for long strings.
