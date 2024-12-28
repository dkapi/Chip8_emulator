#include "cpu.h"

int main(int argc, char* argv[])
{
    if(argc != 2){
        printf("no rom entered\n");
        exit(0);
    }
    const char *rom_file = argv[1];
    cpu_init(rom_file);

    /*
    for(;;){
        emulate_cycle();
        .
        .
        .
        TODO
        
    }
    */

}