## 2024-05-22 - Headless UX: The "Meta" Interface
**Learning:** In headless/serial environments (common in kernel dev), "UX" extends beyond the software itself to the emulator/runner environment. Users often get stuck in QEMU without knowing how to exit.
**Action:** Always include explicit exit instructions (e.g., "Ctrl+A, X") in the boot/halt messages for headless kernels. Visuals like ASCII art become the primary branding mechanism in the absence of a GUI.
