
#include <stddef.h>
#include <stdint.h>
#include "font.h"
#include "splash.h"
#include "rtc.h"

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
    for (int i = 0; i < FONT_HEIGHT; i++) {
        for (int j = 0; j < FONT_WIDTH; j++) {
            if ((glyph[i] >> j) & 1) {
                put_pixel(x + (FONT_WIDTH - 1 - j), y + i, color);
            }
        }
    }
}

void draw_string(const char* str, uint32_t x, uint32_t y, uint32_t color) {
    for (int i = 0; str[i] != '\0'; i++) {
        draw_char(str[i], x + i * FONT_WIDTH, y, color);
    }
}

void draw_splash() {
    uint32_t x_offset = (width - SPLASH_WIDTH) / 2;
    uint32_t y_offset = (height - SPLASH_HEIGHT) / 2;

    for (uint32_t y = 0; y < SPLASH_HEIGHT; y++) {
        for (uint32_t x = 0; x < SPLASH_WIDTH; x++) {
            put_pixel(x_offset + x, y_offset + y, splash_data[y * SPLASH_WIDTH + x]);
        }
    }
}

void delay(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 100000; i++);
}

// A simple itoa function
void itoa(int n, char* s) {
    int i = 0;
    if (n < 0) {
        s[i++] = '-';
        n = -n;
    }
    int start = i;
    do {
        s[i++] = n % 10 + '0';
        n /= 10;
    } while (n);
    s[i] = '\0';

    // Reverse the string
    for (int j = start, k = i - 1; j < k; j++, k--) {
        char temp = s[j];
        s[j] = s[k];
        s[k] = temp;
    }
}

void draw_taskbar() {
    draw_rect(0, height - 40, width, 40, 0x00C0C0C0); // Light Gray

    rtc_time_t time;
    rtc_read_time(&time);

    char date_str[32];
    char year_str[5], month_str[3], day_str[3];
    itoa(time.year, year_str);
    itoa(time.month, month_str);
    itoa(time.day, day_str);

    // Simple concatenation
    int i = 0;
    for (int j=0; year_str[j]; j++) date_str[i++] = year_str[j];
    date_str[i++] = '-';
    for (int j=0; month_str[j]; j++) date_str[i++] = month_str[j];
    date_str[i++] = '-';
    for (int j=0; day_str[j]; j++) date_str[i++] = day_str[j];
    date_str[i] = '\0';

    draw_string(date_str, width - (10 * FONT_WIDTH) - 10, height - 28, 0x00000000);
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
        draw_splash();
        delay(300); // 3-second delay

        clear_screen(0x00008080); // Teal desktop
        draw_taskbar();
    }

    for (;;) {
        __asm__ volatile("hlt");
    }
}
