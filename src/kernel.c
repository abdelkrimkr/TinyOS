
__attribute__((section(".text")))
void kmain(void) {
  volatile unsigned short *vga = (unsigned short *)0xB8000;
  const char *msg = "Hello from tiny OS";
  for (int i = 0; msg[i] != '\0'; ++i) {
    vga[i] = (unsigned short)(0x0F00 | msg[i]); // white on black
  }

  for (;;) {
    __asm__ volatile("hlt");
  }
}
