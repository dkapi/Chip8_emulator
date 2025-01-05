#include "cpu.h"

int main(int argc, char* argv[])
{
    SDL_Surface* screen_surface = NULL;
    SDL_Window* c8_window = gfx_init(PTALL, PWIDE);


    // get window surface
    screen_surface = SDL_GetWindowSurface(c8_window);
    // fill surface with white
    SDL_FillRect(screen_surface, NULL, SDL_MapRGB( screen_surface->format, 0xFF, 0xFF, 0xFF));
    
    // update surface
    SDL_UpdateWindowSurface(c8_window);
    // some cancer to keep window up
    SDL_Event e; bool quit = false; while( quit == false ){ while( SDL_PollEvent( &e ) ){ if( e.type == SDL_QUIT ) quit = true; } }




    // if(argc != 2){
    //     printf("no rom entered\n");
    //     exit(0);
    // }
    // const char *rom_file = argv[1];
    // cpu_init(rom_file);

    // /*
    // for(;;){
    //     emulate_cycle();
    //     .
    //     .
    //     .
    //     TODO
        
    // }
    // */


    SDL_DestroyWindow(c8_window);
    SDL_Quit();

}