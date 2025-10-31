
#include <stddef.h>
#include <stdint.h>
#include "font.h"

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

// Global framebuffer info
uint32_t* framebuffer;
uint32_t pitch;
uint32_t width;
uint32_t height;

void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x < width && y < height) {
        framebuffer[y * (pitch / 4) + x] = color;
    }
}

void clear_screen(uint32_t color) {
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            put_pixel(x, y, color);
        }
    }
}

void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    for (uint32_t j = y; j < y + h; j++) {
        for (uint32_t i = x; i < x + w; i++) {
            put_pixel(i, j, color);
        }
    }
}

void draw_char(char c, uint32_t x, uint32_t y, uint32_t color) {
    const uint8_t* glyph = font[(int)c];
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if ((glyph[i] >> j) & 1) {
                put_pixel(x + (7-j), y + i, color);
            }
        }
    }
}

void draw_string(const char* str, uint32_t x, uint32_t y, uint32_t color) {
    for (int i = 0; str[i] != '\0'; i++) {
        draw_char(str[i], x + i * 8, y, color);
    }
}

void draw_button(const char* text, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    // Draw button background
    draw_rect(x, y, width, height, 0x00808080); // Gray

    // Draw text in the center of the button
    int text_len = 0;
    while(text[text_len] != '\0') text_len++;

    uint32_t text_x = x + (width - (text_len * 8)) / 2;
    uint32_t text_y = y + (height - 8) / 2;
    draw_string(text, text_x, text_y, 0x00FFFFFF); // White
}

void kmain(unsigned long magic, unsigned long addr) {
    (void)magic; // Suppress unused parameter warning

    struct multiboot_tag *tag;
    for (tag = (struct multiboot_tag *)(addr + 8);
         tag->type != 0;
         tag = (struct multiboot_tag *)((uint8_t *)tag + ((tag->size + 7) & ~7))) {
        if (tag->type == 8) { // Framebuffer tag
            struct multiboot_tag_framebuffer *fb_tag = (struct multiboot_tag_framebuffer *)tag;
            framebuffer = (uint32_t*)((uintptr_t)fb_tag->framebuffer_addr);
            pitch = fb_tag->framebuffer_pitch;
            width = fb_tag->framebuffer_width;
            height = fb_tag->framebuffer_height;
            break;
        }
    }

    if (framebuffer) {
        clear_screen(0x000000FF); // Blue
        draw_button("ABC", 100, 100, 80, 40);
    }

    for (;;) {
        __asm__ volatile("hlt");
    }
}
