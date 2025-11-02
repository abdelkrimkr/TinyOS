ENTRY(_start)
SECTIONS
{
  . = 0x100000;     /* load at 1 MiB */
  .boot : {
    KEEP(*(.multiboot_header))
  }
  .text : {
    *(.text*)
  }
  .rodata : { *(.rodata*) }
  .data : { *(.data*) }
  .bss : { *(.bss*) *(COMMON) }
}
