## 2024-05-23 - [Kernel I/O Optimization]
**Learning:** In kernel development, I/O port access (`inb`/`outb`) is significantly slower than memory operations. When writing to a buffered device like a UART 16550, checking the status register for every byte is a performance bottleneck.
**Action:** Use FIFO burst writes (checking status once per 16 bytes) to reduce I/O port read overhead by ~50% or more, depending on the string length. Always ensure FIFO is enabled in hardware initialization before applying this pattern.
