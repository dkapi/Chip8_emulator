#ifndef __CHIP8_CPU__
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"

#define PWIDE 64
#define PTALL 32
#define MEMORYSIZE 4096

enum registers{
    V0,
    V1,
    V2,
    V3,
    V4,
    V5,
    V6,
    V7,
    V8,
    V9,
    VA,
    VB,
    VC,
    VD,
    VE,
    VF,
    REGCOUNT
};



typedef struct chip8_cpu {
    unsigned short opcode;
    unsigned char memory[MEMORYSIZE];
    unsigned char V[REGCOUNT];
    unsigned char gfx[PWIDE * PTALL];
    unsigned short I;
    unsigned short pc;
    unsigned short stack[16];
    unsigned short sp;
    unsigned char key[16];
    unsigned char delay_timer;
    unsigned char sound_timer;

}chip8_t;



#endif /*__CHIP8_CPU__*/