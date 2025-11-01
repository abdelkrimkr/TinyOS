
#include "graphics.h"
#include "font.h"

static uint32_t* framebuffer;
static uint32_t pitch;
static uint32_t width;
static uint32_t height;

void graphics_init(uint32_t* framebuffer_addr, uint32_t fb_pitch, uint32_t fb_width, uint32_t fb_height) {
    framebuffer = framebuffer_addr;
    pitch = fb_pitch;
    width = fb_width;
    height = fb_height;
}

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
