#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

enum vga_color;

size_t strlen(const char* str);
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);
static inline uint16_t vga_entry(unsigned char uc, uint8_t color);
void init_stdio();

void setColor(uint8_t color);
void write(const char* data, size_t size);
void writeStr(const char* data);