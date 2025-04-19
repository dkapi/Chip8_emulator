#include "gfx.h"

SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

SDL_Window* gfx_init(const int width, const int height) 
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be initialized, SDL_ERROR: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Window* w = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                     width, height, SDL_WINDOW_SHOWN);
    if (!w) {
        printf("Window could not be created, SDL_ERROR: %s\n", SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created, SDL_ERROR: %s\n", SDL_GetError());
        exit(1);
    }

    return w;
}

void chip8_render(chip8_t* c8, int scale, bool draw_borders) 
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // black background
    SDL_RenderClear(renderer);

    for (int y = 0; y < PTALL; y++) {
        for (int x = 0; x < PWIDE; x++) {
            int pixel = c8->gfx[x + y * PWIDE];
            SDL_Rect rect = { x * scale, y * scale, scale, scale };

            if (pixel) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white pixel
                SDL_RenderFillRect(renderer, &rect);

                if (draw_borders) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black outline
                    SDL_RenderDrawRect(renderer, &rect);
                }
            }
        }
    }
    
    SDL_RenderPresent(renderer);
}