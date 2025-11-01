
#pragma once
#include <stdint.h>

void graphics_init(uint32_t* framebuffer_addr, uint32_t fb_pitch, uint32_t fb_width, uint32_t fb_height);
void put_pixel(uint32_t x, uint32_t y, uint32_t color);
void clear_screen(uint32_t color);
void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void draw_char(char c, uint32_t x, uint32_t y, uint32_t color);
void draw_string(const char* str, uint32_t x, uint32_t y, uint32_t color);
