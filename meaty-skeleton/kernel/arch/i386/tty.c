#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void scroll_terminal() {

	// for each character in every line in the terminal except the first
	// move it to the line above it
	for (size_t row = 0; row < VGA_HEIGHT; row++) {
		for (size_t col = 0; col < VGA_WIDTH; col++) {
			size_t row_idx = row*VGA_WIDTH;
			size_t next_row_idx = (row+1)*VGA_WIDTH;

			terminal_buffer[row_idx + col] = terminal_buffer[next_row_idx + col];
		}
	}

}

void clear_line(size_t row) {
	for (size_t col = 0; col < VGA_WIDTH; col++) {
		terminal_putentryat(0, terminal_color, col, row);
	}
}

void terminal_putchar(char c) {
	unsigned char uc = c;
	bool newline = false;

	if (c != '\n') {
			terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
	} else {
		newline = true;
	}

	if (++terminal_column == VGA_WIDTH || newline) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT){

			// scroll the terminal instead
			scroll_terminal();
			clear_line(terminal_row);

			// Stay at the bottom
			terminal_row = VGA_HEIGHT - 1;
		}
	}
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}
