#include "display.h"

// Renders the contents of the pixel buffer onto the screen.
void render_frame(Display *display)
{
    for (int row = 0; row < DISPLAY_HEIGHT; row++) {
        for (int col = 0; col < DISPLAY_WIDTH; col++) {
            int color = (display->pixels[row][col] == 1 ? 
                SDL_MapRGB(display->surface->format, 0xFF, 0xFF, 0xFF) :
                SDL_MapRGB(display->surface->format, 0, 0, 0));
            SDL_Rect rect;
            rect.x = WINDOW_SCALE * col;
            rect.y = WINDOW_SCALE * row;
            rect.w = WINDOW_SCALE;
            rect.h = WINDOW_SCALE;
            SDL_FillRect(display->surface, &rect, color);
        }
    }
    SDL_UpdateWindowSurface(display->window);
}

// Resets the pixel buffer back to all 0s (Black).
void clear_pixels(Display *display)
{
    for (int i = 0; i < DISPLAY_HEIGHT; i++) {
        for (int j = 0; j < DISPLAY_WIDTH; j++)
            display->pixels[i][j] = 0;
    }
}

void initialize_display(Display *display)
{
    SDL_Init(SDL_INIT_VIDEO);
    display->window = SDL_CreateWindow(
        "Chip 8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        DISPLAY_WIDTH * WINDOW_SCALE, DISPLAY_HEIGHT * WINDOW_SCALE, SDL_WINDOW_SHOWN
    );
    display->surface = SDL_GetWindowSurface(display->window);
    clear_pixels(display);
}

void destroy_display(Display *display)
{
    SDL_DestroyWindow(display->window);
    SDL_Quit();
}
