
#include "log.h"
#include "graphics.h"
#include "font.h"

#define LOG_WIDTH 80
#define LOG_HEIGHT 25
#define LOG_X_OFFSET 10
#define LOG_Y_OFFSET 10
#define LOG_COLOR_INFO 0xFFFFFF // White
#define LOG_COLOR_ERROR 0xFF0000 // Red

static uint32_t cursor_x = 0;
static uint32_t cursor_y = 0;

void log_init() {
    cursor_x = LOG_X_OFFSET;
    cursor_y = LOG_Y_OFFSET;
}

static void log_print(const char* message, uint32_t color) {
    draw_string(message, cursor_x, cursor_y, color);
    cursor_y += FONT_HEIGHT;
}

void log_info(const char* message) {
    log_print(message, LOG_COLOR_INFO);
}

void log_error(const char* message) {
    log_print(message, LOG_COLOR_ERROR);
}
