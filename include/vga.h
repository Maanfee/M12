#ifndef VGA_H
#define VGA_H

#include "common.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xB8000

typedef enum Color {
    BLACK         = 0x0,
    BLUE          = 0x1,
    GREEN         = 0x2,
    CYAN          = 0x3,
    RED           = 0x4,
    MAGENTA       = 0x5,
    BROWN         = 0x6,
    LIGHT_GRAY    = 0x7,
    DARK_GRAY     = 0x8,
    LIGHT_BLUE    = 0x9,
    LIGHT_GREEN   = 0xA,
    LIGHT_CYAN    = 0xB,
    LIGHT_RED     = 0xC,
    LIGHT_MAGENTA = 0xD,
    LIGHT_BROWN   = 0xE,
    WHITE         = 0xF
} Color;

extern uint8_t color;

void vga_putc(char c);
void set_color(Color new_color);
void kprintcolor(const char* str, Color new);
void rmline();

void Init_VGA();
void VGA_EnableCursor();
void VGA_DisableCursor();
void VGA_GetCursor(int* x, int* y);
void VGA_SetCursor(int x, int y);
uint16_t VGA_GetCursorPosition();
void SetColor(Color new_color);

#endif