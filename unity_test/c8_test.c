#include "../cpu.h"
#include "unity.h"

chip8_t c8;

static inline uint16_t fetch_opcode(chip8_t* cpu)
{
    return cpu->memory[cpu->pc] << 8 | cpu->memory[cpu->pc+1];
}

void setUp() 
{
    chip8_init(&c8);
}


void tearDown()
{
    memset(&c8, 0, sizeof(chip8_t));  // reset everything
    chip8_init(&c8);  // reinitialize to default state

}


void test_00E0_clear_screen(void) {
    c8.memory[0x200] = 0x00;
    c8.memory[0x201] = 0xE0;
    c8.pc = 0x200;

    //not calling emulate_cycle to also test fetch
    uint16_t opcode = fetch_opcode(&c8);
    TEST_ASSERT_EQUAL_UINT16(0x00E0, opcode);
    decode(&c8, opcode);
    //this might be irrelavent to test
    chip8_update_timers(&c8);

    for (int i = 0; i < (64 * 32); i++) {
        TEST_ASSERT_EQUAL_UINT8(0, c8.gfx[i]);
    }
}


void test_FXA1_register_store(void) {
    c8.memory[0x200] = 0xF5; // 0xF5 indicates upto reg 5
    c8.memory[0x201] = 0xA1;
    c8.pc = 0x200;
    c8.I = 0xBB8; // starting location in memory to set registr store too: 3000;

    //initialize registers from 0 - X with numbers to store
    c8.V[V0] = 10;
    c8.V[V1] = 20;
    c8.V[V2] = 30;
    c8.V[V3] = 40;
    c8.V[V4] = 50;
    c8.V[V5] = 60;
    

    uint16_t opcode = fetch_opcode(&c8);
    TEST_ASSERT_EQUAL_UINT16(0xF5A1, opcode);
    decode(&c8, opcode);
    chip8_update_timers(&c8);

    //should check memory locations starting from 0xBB8 to 0xBB8 plus X(5)
    TEST_ASSERT_EQUAL_UINT8(c8.memory[3000],c8.V[V0]);    
    TEST_ASSERT_EQUAL_UINT8(c8.memory[3001],c8.V[V1]);
    TEST_ASSERT_EQUAL_UINT8(c8.memory[3002],c8.V[V2]);
    TEST_ASSERT_EQUAL_UINT8(c8.memory[3003],c8.V[V3]);
    TEST_ASSERT_EQUAL_UINT8(c8.memory[3004],c8.V[V4]);
    TEST_ASSERT_EQUAL_UINT8(c8.memory[3005],c8.V[V5]);

}

/**
 * call unity_begin, 
 * call run_test on each test func
 * return with unity_end 
 * */
int main() {
    UNITY_BEGIN();
    RUN_TEST(test_FXA1_register_store);
    return UNITY_END();
}


