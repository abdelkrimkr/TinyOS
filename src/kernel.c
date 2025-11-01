
#include <stddef.h>
#include <stdint.h>
#include "graphics.h"
#include "window.h"
#include "log.h"
#include "idt.h"
#include "keyboard.h"
#include "terminal.h"
#include "stdio.h"
#include "string.h"

// Kernel configuration
struct kernel_config {
    int splash;
};

static struct kernel_config config = {
    .splash = 1, // Default to splash on
};

static void parse_cmdline(char* cmdline) {
    if (strcmp(cmdline, "splash=off") == 0) {
        config.splash = 0;
    }
}

// Multiboot2 tag structures
struct multiboot_tag {
    uint32_t type;
    uint32_t size;
};

struct multiboot_tag_framebuffer {
    struct multiboot_tag common;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t reserved;
};

struct multiboot_tag_string {
    struct multiboot_tag common;
    char string[];
};

void kmain(unsigned long magic, unsigned long addr) {
    (void)magic; // Suppress unused parameter warning

    struct multiboot_tag *tag;
    struct multiboot_tag_framebuffer *fb_tag = NULL;
    char* cmdline = NULL;

    for (tag = (struct multiboot_tag *)(addr + 8);
         tag->type != 0;
         tag = (struct multiboot_tag *)((uint8_t *)tag + ((tag->size + 7) & ~7))) {
        switch (tag->type) {
            case 1: // Command line
                cmdline = ((struct multiboot_tag_string *)tag)->string;
                break;
            case 8: // Framebuffer
                fb_tag = (struct multiboot_tag_framebuffer *)tag;
                break;
        }
    }

    if (fb_tag) {
        graphics_init((uint32_t*)((uintptr_t)fb_tag->framebuffer_addr),
                      fb_tag->framebuffer_pitch,
                      fb_tag->framebuffer_width,
                      fb_tag->framebuffer_height);

        log_init();
        log_info("Graphics initialized.");
        if (cmdline) {
            parse_cmdline(cmdline);
        }

        if (config.splash) {
            draw_window(50, 50, 400, 300, "My Window");
        }

        idt_init();
        log_info("IDT initialized.");

        keyboard_init();
        log_info("Keyboard initialized.");

        terminal_init();
        log_info("Terminal initialized.");

        terminal_run();

    } else {
        // No framebuffer, what to do?
        // For now, just halt.
    }

    for (;;) {
        __asm__ volatile("hlt");
    }
}
