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


/**
 * call unity_begin, 
 * call run_test on each test func
 * return with unity_end 
 * */
int main() {
    UNITY_BEGIN();
    RUN_TEST(test_00E0_clear_screen);
    return UNITY_END();
}


