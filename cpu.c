#include "cpu.h"


//TODO: maybe dont do this, and just initialize in cpu_init function
static chip8_t cpu = {0}; //short hand to init all to zero


/*  font in chip-8 emulators is stored in
    memory locations 0x050 - 0x0A0 i.e. 80-160
    we will follow that convention here
*/

//extern to only store one instance of this array between files
const uint8_t chip8_font_sprites[] = 
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void load_font()
{
    int i;
    for(i = 0; i < sizeof(chip8_font_sprites); i++) {
        cpu.memory[0x050 + i] = chip8_font_sprites[i];
    }
}
//TODO: possibly use errno.h for error handling across funcs
void load_rom(const char* rom)
{
    FILE *buffer;
    buffer = fopen(rom, "rb");

    if(buffer == NULL){
        printf("ERROR opening ROM\n");
        return;
    }

    fseek(buffer, 0, SEEK_END);
    long bufferSize = ftell(buffer);
    fseek(buffer, 0, SEEK_SET);

    unsigned char *rom_buf = (unsigned char *) malloc(bufferSize);

    fread(rom_buf, 1, bufferSize, buffer);

    int i;
    for(i =0; i< bufferSize; i++){
        cpu.memory[512+i] = rom_buf[i];
    }

    fclose(buffer);
    free(rom_buf);
}


// prolly write it as a pre processor function
static inline uint16_t fetch_opcode()
{
    return cpu.memory[cpu.pc] << 8 | cpu.memory[cpu.pc+1];
}


void decode(uint16_t opcode)
{
    uint16_t NNN    = opcode & 0x0FFF;  // address, 12 bit
    uint16_t NN     = opcode & 0x00FF;  // 8-bit constant
    uint16_t N      = opcode & 0x000F;  // 4-bit constant
    uint16_t regX   = NIBBLE_2(opcode); // 2nd most significant nibble (4-bit register identifier)
    uint16_t regY   = NIBBLE_3(opcode); // 3rd most significant nibble (4-bit register identifier)

    switch(opcode & 0xF000) {
        case 0x0000: 
            switch(opcode & 0x00FF) {
                case 0x00E0: // clears the display
                    break;
                case 0x00EE: // returns from a subroutine
                    if(cpu.pc == 0) {
                        printf("stack underflow error, cannot return from subroutine as there is none");
                        exit(1);
                    }
                    --cpu.sp;
                    cpu.pc = cpu.stack[cpu.sp];
                    break;
                default:  // syscall case, i.e. 0x0NNN, or anything else
                    break;
            }
            break;
        case 0x1000: // jump to address NNN
            cpu.pc = NNN;
            break;
        case 0x2000:
            cpu.stack[cpu.sp] = cpu.pc;
            ++cpu.sp;
            cpu.pc = NNN;
            break;
        case 0x3000: // skip next instruction if reg VX == NN
            if(cpu.V[regX] == NN) cpu.pc += 2; // i think to skip next instruction we just incriment pc by 2
            break;
        case 0x4000: /// skip next instruction if reg VX != NN
            if(cpu.V[regX] != NN) cpu.pc += 2;
            break;
        case 0x5000:
            if(cpu.V[regX] == cpu.V[regY]) cpu.pc += 2;
            break;
        case 0x6000:
            cpu.V[regX] = NN;
            break;
        case 0x7000:
            cpu.V[regX] += NN;
            break;
        default:
        printf("unknown opcode:%d\n",opcode);
        break;
    }


}


void emulate_cycle()
{
    uint16_t opcode = fetch_opcode();
    decode(opcode);
    
}


void cpu_init(const char* rom)
{
    load_font();
    load_rom(rom);
    // after loading of rom file init program counter to 512
    cpu.pc += 512; 
}

void mem_dump()
{
    for(int i =0; i < MEMORYSIZE; i++){
        printf("mem %d: %d\n",i,cpu.memory[i]);
    }
}




