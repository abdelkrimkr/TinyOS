.section .multiboot_header
.align 8
header_start:
    .long 0xe85250d6                /* Magic number (multiboot 2) */
    .long 0                         /* Architecture 0 (protected mode i386) */
    .long header_end - header_start /* Header length */
    /* Checksum */
    .long 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    /* Framebuffer tag */
    .align 8
    .short 5    /* Type: Framebuffer */
    .short 0    /* Flags */
    .long 20    /* Size */
    .long 1024  /* Width */
    .long 768   /* Height */
    .long 32    /* Depth */

    /* End tag */
    .align 8
    .short 0    /* Type */
    .short 0    /* Flags */
    .long 8     /* Size */
header_end:
