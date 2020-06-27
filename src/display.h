#ifndef GRAPHICS_H
#define GRAPHICS_H

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define WINDOW_SCALE 20

#include <stdint.h>
#include <SDL2/SDL.h>

typedef struct display_t {
    uint8_t pixels[DISPLAY_HEIGHT][DISPLAY_WIDTH];
    SDL_Window *window;
    SDL_Surface *surface;
} Display;

void initialize_display(Display *display);
void render_frame(Display *display);
void destroy_display(Display *display);
void clear_pixels(Display *display);
void set_title(Display *display, const char *program);

#endif
