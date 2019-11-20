#ifndef GRAPHICS_H
#define GRAPHICS_H

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define WINDOW_SCALE 20

#include <stdint.h>

uint8_t pixels[DISPLAY_HEIGHT][DISPLAY_WIDTH];

void initialize_renderer();
void render_frame();
void destroy_renderer();
void clear_pixels();

#endif
