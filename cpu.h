#ifndef CPU_H
#define CPU_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "gfx.h"

#define PWIDE 64         // pixel width definition
#define PTALL 32         // pixel length definition
#define MEMORYSIZE 4096  // max size of memory 4KB
#define STACKSIZE 16     // size of internal stack
#define KEYSIZE 16       // amount of keys usable

// macros to return all nibbles of a 2 byte value
#define NIBBLE_1(b) ((b) & 0x0F)          // least significant nibble (4th nibble from the right)
#define NIBBLE_2(b) (((b) >> 4) & 0x0F)   // 2nd nibble from the right
#define NIBBLE_3(b) (((b) >> 8) & 0x0F)   // 3rd nibble from the right
#define NIBBLE_4(b) (((b) >> 12) & 0x0F)  // most significant nibble (1st nibble from the right)
#define FETCH_OPCODE() (cpu.memory[cpu.pc] << 8 | cpu.memory[cpu.pc + 1]) //test this 


/**
 *  0x000-0x1FF - Chip 8 interpreter (contains font set in memory)
 *  0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
 *  0x200-0xFFF - Program ROM and work RAM
**/

extern const uint8_t chip8_font_sprites[];

enum registers{   //enumerators for each register of the chipset
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
    VF,      //used as flag for some instructions, carry/borrow flag, and pixel collision
    REGCOUNT
};



typedef struct chip8_cpu {
    uint16_t opcode;              // current opcode
    uint16_t I;                   // index/address register 12 bit wide
    uint16_t pc;                  // program counter: range from 0x00 - 0xFFF
    uint16_t stack[STACKSIZE];           // stack for jumps/routine calls
    uint16_t sp;                  // stack pointer
    uint8_t memory[MEMORYSIZE];   // memory/ram of chipset
    uint8_t V[REGCOUNT];          // virtual registars 0-16
    uint8_t gfx[PWIDE * PTALL];   // aray to hold pixel state(1 or 0)
    bool drawflag;                // flag to update screen
    uint8_t key[KEYSIZE];              // keypad array: use to store current state
    uint8_t delay_timer;          // timer register intended for timing events in programs
    uint8_t sound_timer;          // timer reigster intended for sound effects, nonzero value
                                  // makes a beeping sound

}chip8_t;



//function prototypes

void chip8_init(chip8_t *cpu);
//inline uint16_t fetch_opcode(chip8_t* cpu);
void load_font(chip8_t *cpu);
void load_rom(chip8_t *cpu, const char* rom);
void decode(chip8_t *cpu, uint16_t opcode);
void emulate_cycle(chip8_t *cpu);
void chip8_update_timers(chip8_t *cpu);
// void cpu_init(chip8_t *cpu, const char* rom);
// for load and store, address is to remain unmodified, but incremented by 1 in each iteration
void register_store(chip8_t *cpu, uint8_t start_reg, const uint16_t address);
void register_load(chip8_t *cpu, uint8_t start_reg, const uint16_t address);
void mem_dump(chip8_t *cpu);
void dump_gfx(chip8_t* cpu);
void draw_gfx(chip8_t *cpu, uint8_t Vx, uint8_t Vy, uint16_t height);


#endif /*CPU_H*/