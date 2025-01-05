#include "gfx.h"



SDL_Window* gfx_init(const int ptall, const int pwide) {
   SDL_Window* w = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pwide, ptall, SDL_WINDOW_SHOWN);

    if(w== NULL) {
    printf("Error, window could not be initialized, SDL_ERROR: %s\n", SDL_GetError());
    exit(1);
    }
    return w;
}