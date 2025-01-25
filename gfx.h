#ifndef GFX_H
#define GFX_H

#include <SDL2/SDL.h>
#include "cpu.h"

// forward declaration of chip8 struct
typedef struct chip8_cpu chip8_t;


// global SDL variables
extern SDL_Renderer* renderer;
extern SDL_Texture* texture;

// functio prototypes
SDL_Window* gfx_init(int ptall, int pwide);
void chip8_update_texture(chip8_t* c8);


#endif /* GFX_H */