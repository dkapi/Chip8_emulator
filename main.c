#include "cpu.h"
#include "gfx.h"

int main(int argc, char* argv[])
{
    // grab rom file from arguments
    if(argc != 2){
        printf("no rom entered\n");
        exit(0);
    }
    const char *rom_file = argv[1];

    //initialize chip 8 emulator
    chip8_t* c8 = malloc(sizeof(chip8_t));
    chip8_init(c8, rom_file);

    // initialize sdl and create window
    SDL_Window* c8_window = gfx_init(PTALL * 20, PWIDE * 20);

    // main loop and SDL event struct
    SDL_Event e;
    bool quit = false;

    while(!quit) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                quit = true;
            }
        }
        // emulate a single cycle of the chip 8 cpu
        emulate_cycle(c8);

        // if drawflag is set, update and render the screen
        if (c8->drawflag) {
            chip8_update_texture(c8);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            c8->drawflag = false;
        }
        dump_gfx(c8);

        //delay for ~60Hz
        SDL_Delay(16); 
    }

    SDL_DestroyWindow(c8_window);
    SDL_Quit();
    free(c8);

}