#include "cpu.h"

static chip8_t cpu = {0}; //short hand to init all to zero

const uint8_t font_sprites[] = 
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

/*  font in most chip-8 emulators is stored in
    memory locations 0x050 - 0x0A0 i.e. 80-160
    we will follow that convention here
*/
void load_font()
{
    int i;
    for(i = 0; i < sizeof(font_sprites); i++){
        cpu.memory[0x050 + i] = font_sprites[i];
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

// pretty sure this should be short as opcode is 2 bytes
// need to double check
unsigned short fetch_opcode()
{
    unsigned short opcode;
    return opcode = cpu.memory[cpu.pc] << 8| cpu.memory[cpu.pc+1];
}

void emulate_cycle()
{
    short opcode = fetch_opcode();

    switch (opcode & 0xF0)
    {
    case /* constant-expression */:
        /* code */
        break;
    
    default:
        break;
    }
}


void sys_init(const char* rom)
{
    load_font();
    load_rom(rom);
    // after loading of rom file init program counter to 512
    cpu.pc += 512; 
}

void mem_dump()
{
    for(int i =0; i < 4097; i++){
        printf("mem %d: %d\n",i,cpu.memory[i]);
    }
}

int main(int argc, char* argv[])
{
    if(argc != 2){
        printf("no rom entered\n");
        exit(0);
    }
    const char *rom_file = argv[1];
    sys_init(rom_file);
    // unsigned short opcode1 = fetch_opcode();
    // uint16_t opcode2 = cpu.memory[cpu.pc] << 8 | cpu.memory[cpu.pc+1];

    // printf("short opcode1: %x\n", opcode1);
    // printf("unsigned 16: %x\n", opcode2);
}



