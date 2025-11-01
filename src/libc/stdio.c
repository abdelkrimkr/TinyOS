
#include "stdio.h"
#include "graphics.h"
#include "string.h"
#include "font.h"
#include <stdarg.h>

static uint32_t printf_x = 10;
static uint32_t printf_y = 10;

static void print_char(char c) {
    if (c == '\n') {
        printf_x = 10;
        printf_y += FONT_HEIGHT;
    } else {
        draw_char(c, printf_x, printf_y, 0xFFFFFF);
        printf_x += FONT_WIDTH;
    }
}

static void print_string(const char* s) {
    for (int i = 0; s[i]; i++) {
        print_char(s[i]);
    }
}

static void print_int(int n) {
    if (n < 0) {
        print_char('-');
        n = -n;
    }

    if (n == 0) {
        print_char('0');
        return;
    }

    char buf[12];
    int i = 0;
    while (n > 0) {
        buf[i++] = (n % 10) + '0';
        n /= 10;
    }

    while (i-- > 0) {
        print_char(buf[i]);
    }
}

void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (int i = 0; fmt[i]; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case 's':
                    print_string(va_arg(args, const char*));
                    break;
                case 'd':
                    print_int(va_arg(args, int));
                    break;
                default:
                    print_char(fmt[i]);
                    break;
            }
        } else {
            print_char(fmt[i]);
        }
    }

    va_end(args);
}
