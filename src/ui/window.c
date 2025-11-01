
#include "window.h"
#include "graphics.h"
#include "color.h"
#include "font.h"
#include "string.h"

#define TITLE_BAR_HEIGHT 20
#define TITLE_COLOR COLOR_WHITE

void draw_window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const char* title) {
    // Draw the main window body
    draw_rect(x, y, width, height, COLOR_LIGHT_GRAY);

    // Draw the title bar
    draw_rect(x, y, width, TITLE_BAR_HEIGHT, COLOR_BLUE);

    // Draw the title text
    uint32_t title_x = x + (width / 2) - (strlen(title) * FONT_WIDTH / 2);
    uint32_t title_y = y + (TITLE_BAR_HEIGHT / 2) - (FONT_HEIGHT / 2);
    draw_string(title, title_x, title_y, TITLE_COLOR);
}
