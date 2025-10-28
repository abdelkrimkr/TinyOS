ENTRY(_start)
SECTIONS
{
  . = 0x100000;     /* load at 1 MiB */
  .text : { *(.multiboot) *(.text*) }
  .rodata : { *(.rodata*) }
  .data : { *(.data*) }
  .bss : { *(.bss*) *(COMMON) }
}
