#ifndef __CHIP8_CPU__

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
    COUNT
};



typedef struct chip8_cpu {
    unsigned short opcode;
    unsigned char memory[4096];
    unsigned char V[COUNT];
    unsigned short I;
    unsigned short pc;
    unsigned short stack[16];
    unsigned short sp;
    unsigned char key[16];
    unsigned char delay_timer;
    unsigned char sound_timer;

}chip8_t;



#endif /*__CHIP8_CPU__*/