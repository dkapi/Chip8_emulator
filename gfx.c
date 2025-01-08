#include "gfx.h"

SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

SDL_Window* gfx_init(const int ptall, const int pwide) 
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL count not be initialized, SDL_ERROR:%s\n", SDL_GetError());
        exit(1);
    }
    
    SDL_Window* w = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pwide, ptall, SDL_WINDOW_SHOWN);
    if(w== NULL) {
    printf("error, window could not be created, SDL_ERROR: %s\n", SDL_GetError());
    exit(1);
    }

    renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL) {
        printf("renderer could not be created, SDL_ERROR: %s\n", SDL_GetError());
        exit(1);
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, PWIDE, PTALL);
        if(texture == NULL) {
        printf("texture could not be created, SDL_ERROR: %s\n", SDL_GetError());
        exit(1);
    }

    return w;
}

void chip8_update_texture(chip8_t* c8) 
{
    uint32_t pixels[PTALL * PWIDE];

    for (int y = 0; y < PTALL; y++) {
        for (int x = 0; x < PWIDE; x++) {
            uint8_t pixel = c8->gfx[x + y * PWIDE]; 
            pixels[y * PWIDE + x] = pixel ? 0xFFFFFFFF : 0x000000FF;

            // // Debug: Check pixel values
            // if (pixel) {
            //     printf("Pixel set at (%d, %d)\n", x, y);
            // }
        }
    }

    SDL_UpdateTexture(texture, NULL, pixels, PWIDE * sizeof(uint32_t));
}