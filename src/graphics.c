#include <SDL2/SDL.h>
#include "graphics.h"

Uint32 white, black;
SDL_Window *window = NULL;
SDL_Surface *surface = NULL;

// Renders the contents of the pixel buffer onto the screen.
void render_frame()
{
    for (int row = 0; row < DISPLAY_HEIGHT; row++) {
        for (int col = 0; col < DISPLAY_WIDTH; col++) {
            int color = pixels[row][col] == 1 ? white : black;
            SDL_Rect rect;
            rect.x = WINDOW_SCALE * col;
            rect.y = WINDOW_SCALE * row;
            rect.w = WINDOW_SCALE;
            rect.h = WINDOW_SCALE;
            SDL_FillRect(surface, &rect, color);
        }
    }
    SDL_UpdateWindowSurface(window);
}

// Resets the pixel buffer back to all 0s (Black).
void clear_pixels()
{
    for (int i = 0; i < DISPLAY_HEIGHT; i++) {
        for (int j = 0; j < DISPLAY_WIDTH; j++)
            pixels[i][j] = 0;
    }
}

void initialize_renderer()
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(
        "Chip 8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        DISPLAY_WIDTH * WINDOW_SCALE, DISPLAY_HEIGHT * WINDOW_SCALE, SDL_WINDOW_SHOWN
    );
    surface = SDL_GetWindowSurface(window);
    white = SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF);
    black = SDL_MapRGB(surface->format, 0, 0, 0);
}

void destroy_renderer()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}
