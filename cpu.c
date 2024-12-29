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

// function to load into start_reg to VF from memory address 
void register_load(uint8_t start_reg, const uint16_t address) 
{
    int i;
    int addr = address;
    for(i = start_reg; i < REGCOUNT; i++) { // maybe add bounds check to starting register (0-15)
        cpu.V[i] = cpu.memory[addr];
        addr++;
    }
}

//function to store into start_reg to VF from memory address
void register_store(uint8_t start_reg, const uint16_t address)
{
    int i;
    int addr = address;
    for(i = start_reg; i < REGCOUNT; i++) {
        cpu.memory[addr] = cpu.V[i];
        addr++;
    }
}

void draw_gfx(uint8_t Vx, uint8_t Vy, uint16_t height) 
{
    uint8_t pixel;
    int x, y;

    //reset VF register
    cpu.V[VF] = 0;

    for(y = 0; y < height; y++) {
        pixel = cpu.memory[cpu.I + y];
        for(x = 0; x < 8; x++) {
            if((pixel & (0x80 >> x)) !=0) {
                if(cpu.gfx[(Vx + x + ((Vy + y) * 64))] == 1) {
                    cpu.V[VF] = 1;
                }
                cpu.gfx[Vx + x + ((Vy + y) * 64)] ^= 1;
            }
        }
    }
    cpu.drawflag = 1;
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
        case 0x2000: // calls subroutine at address NNN
            cpu.stack[cpu.sp] = cpu.pc;
            ++cpu.sp;
            cpu.pc = NNN;
            break;
        case 0x3000: // skip next instruction if reg VX == NN
            if(cpu.V[regX] == NN) cpu.pc += 4; // i think to skip next instruction we just incriment pc by 4, which is 4 bytes, which is 2 opcodes
            break;
        case 0x4000: /// skip next instruction if reg VX != NN
            if(cpu.V[regX] != NN) cpu.pc += 4;
            break;
        case 0x5000: // skips next instruction if VX equals VY
            if(cpu.V[regX] == cpu.V[regY]) cpu.pc += 4;
            break;
        case 0x6000: // sets VX to NN
            cpu.V[regX] = NN;
            cpu.pc +=2;
            break;
        case 0x7000: // adds NN to VX(carry flag unchanged)
            cpu.V[regX] += NN;
            cpu.pc += 2;
            break;
        case 0x8000: // math operators
            switch(opcode & 0x000F) {
                case 0x0: // sets Vx to the value of Vy
                    cpu.V[regX] = cpu.V[regY];
                    cpu.pc += 2;
                    break;
                case 0x1: // sets VX to (VX | VY) (bitwise OR operation)
                    cpu.V[regX] |= cpu.V[regY];
                    cpu.pc += 2;
                    break;
                case 0x2: // sets Vx to (VX & VY) (bitwise AND operation)
                    cpu.V[regX] &= cpu.V[regY];
                    cpu.pc += 2;
                    break;
                case 0x3: // sets VX to (VX ^ VY) (bitwise XOR operation)
                    cpu.V[regX] ^= cpu.V[regY];
                    cpu.pc += 2;
                    break;
                case 0x4: // adds VX to VX, sets VF to 1 if there is an overflow, else 0
                    if (cpu.V[regY] > (0xFF - cpu.V[regX])) {
                        cpu.V[VF] = 1; // carry
                    } else {
                        cpu.V[VF] = 0; // no carry
                    }
                    cpu.V[regX] += cpu.V[regY];
                    cpu.pc += 2; 
                    break;
                case 0x5: // subtracts Vy from Vx, VF is set to 0 when there is an underflow, and 1 when there is not
                    if (cpu.V[regX] >= cpu.V[regY]) {
                        cpu.V[VF] = 1; // no borrow
                    } else {
                        cpu.V[VF] = 0; // borrow 
                    }
                    cpu.V[regX] -= cpu.V[regY];
                    cpu.pc += 2; 
                    break;
                case 0x6: // bitwise right shit VX by 1, stores the least significant bit of Vx prior to the shift into VF
                //TODO: this opcode is ambiguous, may need to add compatibility for the other varient opcode procedure
                    cpu.V[VF] = (cpu.V[regX] & 0x1);
                    cpu.V[regX] >>= 1;
                    cpu.pc += 2;
                    break;
                case 0x7: // sets VX to VY - VX, VF is set to 0, when there is an underflow, else 1
                    if (cpu.V[regY] >= cpu.V[regX]) {
                        cpu.V[VF] = 1; // no borrow
                    } else {
                        cpu.V[VF] = 0; // borrow
                    }
                    cpu.V[regX] = cpu.V[regY] - cpu.V[regX];
                    cpu.pc += 2;
                    break;
                case 0xE: // logical left shift by 1, store MSB of VX in VF
                    cpu.V[VF] = (cpu.V[regX] >> 7) & 0x1;
                    cpu.V[regX] <<= 1;
                    cpu.pc += 2;
                    break;
            }
        case 0x9000: // skips next instruction if VX != VY
            if(cpu.V[regX] != cpu.V[regY]) {
                cpu.pc += 4;
            }
            cpu.pc += 2;
            break;
        case 0xA000: // sets I to the address NNN
            cpu.I = NNN;
            cpu.pc += 2;
            break;
        case 0xB000: // jumps to the address NNN plus reg V0
            cpu.pc = cpu.V[V0] + NNN;
            break;
        case 0xC000: //  sets VX to the result of a bitwise and operation on a ranndom number in the range of (0-255) and NN
            cpu.V[regX] = ((rand() % 255 + 1) & NN);
            cpu.pc += 2;
            break;
        case 0xD000: // draws a sprite at the coordinate (Vx, Vy) //TODO:
            draw_gfx(regX, regY, N);
            cpu.pc += 2;
            break;
        case 0xE000: // key ops
            switch(opcode & 0x000F) {
                //TODO:
                case 0xE: // skips the next instruction if the key stored in VX is pressed
                    if(cpu.key[cpu.V[regX]] != 0) {
                        cpu.pc += 4;
                    } else {
                        cpu.pc += 2;
                    }
                    break;
                case 0xA: // skips the next instruction if the key stroed in VX is not pressed
                    if(cpu.key[cpu.V[regX]] == 0) {
                        cpu.pc += 4;
                    } else {
                        cpu.pc += 2;
                    }
                    break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x07: // sets Vx to the value of the delay timer
                    cpu.V[regX] = cpu.delay_timer;
                    cpu.pc += 2;
                    break;
                case 0x0A: // a key press is awaited, then stored in VX(blocking op, everything but timers halted)
                case 0x15:
                    cpu.delay_timer = cpu.V[regX];
                    cpu.pc += 2;
                    break;
                case 0x18:
                    cpu.sound_timer = cpu.V[regX];
                    cpu.pc += 2;
                    break;
                case 0x1E:
                    cpu.I += cpu.V[regX];
                    break;
                case 0x29: // sets I to the location of the sprite for the character in VX, TODO: this may need more thatn just simply setting location
                    cpu.I = chip8_font_sprites[regX];
                    cpu.pc += 2;
                    break;
                case 0x33: // stores binary decimal representation of VX at addresses 1, 1+1, and 1+2
                    cpu.memory[cpu.I]     =  cpu.V[regX] / 100;
                    cpu.memory[cpu.I + 1] = (cpu.V[regX] / 10)  % 10;
                    cpu.memory[cpu.I + 2] = (cpu.V[regX] / 100) % 10;
                    cpu.pc += 2;                 
                    break;
                case 0x55: 
                    register_store(regX, cpu.I);
                    cpu.pc += 2;
                    break;
                case 0x65:
                    register_load(regX, cpu.I);
                    cpu.pc += 2;
                    break;
            }
            break;
        default:
        printf("unknown opcode:%d\n",opcode);
        break;
    }


}

void update_chip_timers() 
{
    if(cpu.delay_timer > 0) {
        --cpu.delay_timer;
    }

    if(cpu.sound_timer > 0) {
        if(cpu.sound_timer == 1) {
            //printf("BEEP!\n"); // the timer beeps at one, so do something ig
        }
        --cpu.sound_timer;
    }
}


void emulate_cycle()
{
    uint16_t opcode = fetch_opcode();
    decode(opcode); // decode also executes

    //update timers as well
    update_chip_timers();

}


void cpu_init(const char* rom)
{
    // also init memory and set members
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




