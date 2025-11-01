
#include "terminal.h"
#include "keyboard.h"
#include "graphics.h"

#define BUFFER_SIZE 256

static char command_buffer[BUFFER_SIZE];
static int buffer_idx = 0;

void terminal_init() {
    // Clear the buffer
    for (int i=0; i<BUFFER_SIZE; i++) {
        command_buffer[i] = 0;
    }
}

static void handle_command() {
    // For now, just echo the command
    draw_string(command_buffer, 10, 50, 0xFFFFFFFF);

    // Clear the buffer
    for (int i=0; i<BUFFER_SIZE; i++) {
        command_buffer[i] = 0;
    }
    buffer_idx = 0;

    // Redraw the screen
    clear_screen(0x00000000); // Black
    draw_string(">", 10, 10, 0xFFFFFFFF);
}

void terminal_run() {
    clear_screen(0x00000000); // Black
    draw_string(">", 10, 10, 0xFFFFFFFF);

    char c;
    while(1) {
        c = keyboard_getchar();
        if (c != 0) {
            if (c == '\n') {
                handle_command();
            } else if (c == '\b') {
                if (buffer_idx > 0) {
                    buffer_idx--;
                    command_buffer[buffer_idx] = 0;
                    // Redraw the line
                    draw_rect(10 + 8, 10, 800, 16, 0x00000000);
                    draw_string(command_buffer, 10 + 8, 10, 0xFFFFFFFF);
                }
            } else {
                if (buffer_idx < BUFFER_SIZE - 1) {
                    command_buffer[buffer_idx++] = c;
                    draw_string(command_buffer, 10 + 8, 10, 0xFFFFFFFF);
                }
            }
        }
    }
}
