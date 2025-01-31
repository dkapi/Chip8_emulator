#include "cpu.h"
#include "gfx.h"

//sdl keybinds
const SDL_Scancode chip8_keymap[16] = {
    SDL_SCANCODE_X,      // 0
    SDL_SCANCODE_1,      // 1
    SDL_SCANCODE_2,      // 2
    SDL_SCANCODE_3,      // 3
    SDL_SCANCODE_Q,      // 4
    SDL_SCANCODE_W,      // 5
    SDL_SCANCODE_E,      // 6
    SDL_SCANCODE_A,      // 7
    SDL_SCANCODE_S,      // 8
    SDL_SCANCODE_D,      // 9
    SDL_SCANCODE_Z,      // A
    SDL_SCANCODE_C,      // B
    SDL_SCANCODE_4,      // C
    SDL_SCANCODE_R,      // D
    SDL_SCANCODE_F,      // E
    SDL_SCANCODE_V       // F
};

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
    chip8_init(c8);
    load_rom(c8 ,rom_file);

    // initialize sdl and create window
    SDL_Window* c8_window = gfx_init(PTALL * 20, PWIDE * 20);

    // main loop and SDL event struct
    SDL_Event e;
    bool quit = false;

    while(!quit) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                SDL_Scancode scancode = e.key.keysym.scancode;
                bool is_pressed = (e.type == SDL_KEYDOWN);

                for( int i = 0; i < KEYSIZE; i++) {
                    if(scancode == chip8_keymap[i]) {
                        c8->key[i] = is_pressed ? 1 : 0;
                        break;
                    }
                }
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
        //SDL_Delay(16); 
    }

    SDL_DestroyWindow(c8_window);
    SDL_Quit();
    free(c8);

}