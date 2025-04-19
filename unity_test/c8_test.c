#include "../cpu.h"
#include "unity.h"

chip8_t c8;

static inline uint16_t fetch_opcode(chip8_t* cpu) {
    return cpu->memory[cpu->pc] << 8 | cpu->memory[cpu->pc+1];
}

void setUp() {
    chip8_init(&c8);
}

void tearDown() {
    memset(&c8, 0, sizeof(chip8_t));
    chip8_init(&c8);
}

void test_00E0_clear_screen(void) {
    c8.memory[0x200] = 0x00;
    c8.memory[0x201] = 0xE0;
    c8.pc = 0x200;

    decode(&c8, fetch_opcode(&c8));

    for (int i = 0; i < (64 * 32); i++) {
        TEST_ASSERT_EQUAL_UINT8(0, c8.gfx[i]);
    }
    TEST_ASSERT_TRUE(c8.drawflag);
}

void test_00EE_return_subroutine(void) {
    c8.stack[0] = 0x300;
    c8.sp = 1;
    c8.memory[0x200] = 0x00;
    c8.memory[0x201] = 0xEE;
    c8.pc = 0x200;

    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT16(0x300, c8.pc);
    TEST_ASSERT_EQUAL_UINT8(0, c8.sp);
}

void test_1NNN_jump(void) {
    c8.memory[0x200] = 0x12;
    c8.memory[0x201] = 0x34;
    c8.pc = 0x200;

    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT16(0x234, c8.pc);
}

void test_2NNN_call_subroutine(void) {
    c8.memory[0x200] = 0x22;
    c8.memory[0x201] = 0x34;
    c8.pc = 0x200;

    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT16(0x234, c8.pc);
    TEST_ASSERT_EQUAL_UINT16(0x202, c8.stack[0]);
    TEST_ASSERT_EQUAL_UINT8(1, c8.sp);
}

void test_3XNN_skip_if_equal(void) {
    c8.memory[0x200] = 0x30;
    c8.memory[0x201] = 0x10;
    c8.V[0] = 0x10;
    c8.pc = 0x200;

    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT16(0x204, c8.pc);
}

void test_4XNN_skip_if_not_equal(void) {
    c8.memory[0x200] = 0x40;
    c8.memory[0x201] = 0x10;
    c8.V[0] = 0x20;
    c8.pc = 0x200;

    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT16(0x204, c8.pc);
}

void test_5XY0_skip_if_VX_eq_VY(void) {
    c8.memory[0x200] = 0x50;
    c8.memory[0x201] = 0x10;
    c8.V[0] = 0xAB;
    c8.V[1] = 0xAB;
    c8.pc = 0x200;

    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT16(0x204, c8.pc);
}

void test_6XNN_set_VX(void) {
    c8.memory[0x200] = 0x6A;
    c8.memory[0x201] = 0x7F;
    c8.pc = 0x200;

    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT8(0x7F, c8.V[0xA]);
}

void test_7XNN_add_to_VX(void) {
    c8.memory[0x200] = 0x7B;
    c8.memory[0x201] = 0x0A;
    c8.V[0xB] = 0x05;
    c8.pc = 0x200;

    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT8(0x0F, c8.V[0xB]);
}

void test_8XY0_set(void) {
    c8.memory[0x200] = 0x80;
    c8.memory[0x201] = 0x10;
    c8.V[1] = 0x22;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT8(0x22, c8.V[0]);
}

void test_8XY4_add_with_carry(void) {
    c8.memory[0x200] = 0x80;
    c8.memory[0x201] = 0x14;
    c8.V[0] = 0xF0;
    c8.V[1] = 0x20;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT8(0x10, c8.V[0]);
    TEST_ASSERT_EQUAL_UINT8(1, c8.V[VF]);
}

void test_9XY0_skip_if_VX_neq_VY(void) {
    c8.memory[0x200] = 0x90;
    c8.memory[0x201] = 0x10;
    c8.V[0] = 0x11;
    c8.V[1] = 0x22;
    c8.pc = 0x200;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT16(0x204, c8.pc);
}

void test_ANNN_set_I(void) {
    c8.memory[0x200] = 0xA2;
    c8.memory[0x201] = 0xF0;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT16(0x2F0, c8.I);
}

void test_BNNN_jump_V0_plus_addr(void) {
    c8.V[0] = 0x10;
    c8.memory[0x200] = 0xB2;
    c8.memory[0x201] = 0x00;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT16(0x210, c8.pc);
}

void test_CXNN_rand_and_mask(void) {
    c8.memory[0x200] = 0xC0;
    c8.memory[0x201] = 0x0F;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_TRUE(c8.V[0] <= 0x0F);
}

void test_DXYN_draw_gfx_sets_drawflag(void) {
    c8.memory[0x200] = 0xD0;
    c8.memory[0x201] = 0x11;
    c8.I = 0x300;
    c8.memory[0x300] = 0xFF;
    c8.V[0] = 0;
    c8.V[1] = 0;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_TRUE(c8.drawflag);
}

void test_EX9E_skip_if_key_pressed(void) {
    c8.V[1] = 5;
    c8.key[5] = 1;
    c8.memory[0x200] = 0xE1;
    c8.memory[0x201] = 0x9E;
    c8.pc = 0x200;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT16(0x204, c8.pc);
}

void test_EXA1_skip_if_key_not_pressed(void) {
    c8.V[2] = 3;
    c8.key[3] = 0;
    c8.memory[0x200] = 0xE2;
    c8.memory[0x201] = 0xA1;
    c8.pc = 0x200;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT16(0x204, c8.pc);
}

void test_FX07_get_delay_timer(void) {
    c8.delay_timer = 0x55;
    c8.memory[0x200] = 0xF3;
    c8.memory[0x201] = 0x07;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT8(0x55, c8.V[3]);
}

void test_FX15_set_delay_timer(void) {
    c8.V[3] = 0x77;
    c8.memory[0x200] = 0xF3;
    c8.memory[0x201] = 0x15;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT8(0x77, c8.delay_timer);
}

void test_FX18_set_sound_timer(void) {
    c8.V[4] = 0x66;
    c8.memory[0x200] = 0xF4;
    c8.memory[0x201] = 0x18;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT8(0x66, c8.sound_timer);
}

void test_FX1E_add_I(void) {
    c8.I = 0x010;
    c8.V[5] = 0x20;
    c8.memory[0x200] = 0xF5;
    c8.memory[0x201] = 0x1E;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT16(0x30, c8.I);
}

void test_FX29_font_sprite_addr(void) {
    c8.V[6] = 0x5;
    c8.memory[0x200] = 0xF6;
    c8.memory[0x201] = 0x29;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT16(0x069, c8.I);
}

void test_FX33_bcd_conversion(void) {
    c8.V[7] = 234;
    c8.I = 0x300;
    c8.memory[0x200] = 0xF7;
    c8.memory[0x201] = 0x33;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT8(2, c8.memory[0x300]);
    TEST_ASSERT_EQUAL_UINT8(3, c8.memory[0x301]);
    TEST_ASSERT_EQUAL_UINT8(4, c8.memory[0x302]);
}

void test_FX55_store_registers(void) {
    c8.I = 0x300;
    c8.V[0] = 1;
    c8.V[1] = 2;
    c8.memory[0x200] = 0xF1;
    c8.memory[0x201] = 0x55;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT8(1, c8.memory[0x300]);
    TEST_ASSERT_EQUAL_UINT8(2, c8.memory[0x301]);
}

void test_FX65_load_registers(void) {
    c8.I = 0x300;
    c8.memory[0x300] = 5;
    c8.memory[0x301] = 6;
    c8.memory[0x200] = 0xF1;
    c8.memory[0x201] = 0x65;
    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT8(5, c8.V[0]);
    TEST_ASSERT_EQUAL_UINT8(6, c8.V[1]);
}

//keybind tests
void test_EXA1_dont_skip_if_key_is_pressed(void) {
    c8.pc = 0x200;
    c8.V[4] = 2;
    c8.key[2] = 1; // Key is pressed
    c8.memory[0x200] = 0xE4;
    c8.memory[0x201] = 0xA1;

    decode(&c8, fetch_opcode(&c8));
    TEST_ASSERT_EQUAL_UINT16(0x202, c8.pc); // Should NOT skip
}

//drawing tests
void test_drawgfx_basic_sprite_no_collision(void) {
    chip8_init(&c8);
    c8.I = 0x300;
    c8.V[0] = 0;  // V0 = x
    c8.V[1] = 0;  // V1 = y

    // Sprite for the number "0" (from font)
    c8.memory[0x300] = 0xF0;  // ####
    c8.memory[0x301] = 0x90;  // #..#
    c8.memory[0x302] = 0x90;  // #..#
    c8.memory[0x303] = 0x90;  // #..#
    c8.memory[0x304] = 0xF0;  // ####

    draw_gfx(&c8, 0, 1, 5);  // draw at V0, V1, height = 5

    TEST_ASSERT_EQUAL_UINT8(0, c8.V[VF]); // no collision
    TEST_ASSERT_EQUAL_UINT8(1, c8.gfx[0]); // top-left pixel should be on
    TEST_ASSERT_EQUAL_UINT8(1, c8.gfx[1]);
    TEST_ASSERT_EQUAL_UINT8(1, c8.gfx[2]);
    TEST_ASSERT_EQUAL_UINT8(1, c8.gfx[3]);
}


void test_drawgfx_collision_flag(void) {
    chip8_init(&c8);
    c8.I = 0x300;
    c8.V[0] = 0;
    c8.V[1] = 0;

    c8.memory[0x300] = 0xF0;  // ####

    // First draw: turns on pixels
    draw_gfx(&c8, 0, 1, 1);
    TEST_ASSERT_EQUAL_UINT8(0, c8.V[VF]);

    // Second draw: flips the same pixels off = collision
    draw_gfx(&c8, 0, 1, 1);
    TEST_ASSERT_EQUAL_UINT8(1, c8.V[VF]);
}

void test_drawgfx_horizontal_wrap(void) {
    chip8_init(&c8);
    c8.I = 0x300;
    c8.V[0] = 62; // near edge (screen width = 64)
    c8.V[1] = 0;
    c8.memory[0x300] = 0xFF; // 8 pixels on

    draw_gfx(&c8, 0, 1, 1);

    // Should wrap 6 pixels from 62 → 63, then 0 → 3
    TEST_ASSERT_EQUAL_UINT8(1, c8.gfx[62]);
    TEST_ASSERT_EQUAL_UINT8(1, c8.gfx[63]);
    TEST_ASSERT_EQUAL_UINT8(1, c8.gfx[0]);
    TEST_ASSERT_EQUAL_UINT8(1, c8.gfx[1]);
    TEST_ASSERT_EQUAL_UINT8(1, c8.gfx[2]);
    TEST_ASSERT_EQUAL_UINT8(1, c8.gfx[3]);
}

int main() 
{
    UNITY_BEGIN();
    RUN_TEST(test_00E0_clear_screen);
    RUN_TEST(test_00EE_return_subroutine);
    RUN_TEST(test_1NNN_jump);
    RUN_TEST(test_2NNN_call_subroutine);
    RUN_TEST(test_3XNN_skip_if_equal);
    RUN_TEST(test_4XNN_skip_if_not_equal);
    RUN_TEST(test_5XY0_skip_if_VX_eq_VY);
    RUN_TEST(test_6XNN_set_VX);
    RUN_TEST(test_7XNN_add_to_VX);
    RUN_TEST(test_8XY0_set);
    RUN_TEST(test_8XY4_add_with_carry);
    RUN_TEST(test_9XY0_skip_if_VX_neq_VY);
    RUN_TEST(test_ANNN_set_I);
    RUN_TEST(test_BNNN_jump_V0_plus_addr);
    RUN_TEST(test_CXNN_rand_and_mask);
    RUN_TEST(test_DXYN_draw_gfx_sets_drawflag);
    RUN_TEST(test_EX9E_skip_if_key_pressed);
    RUN_TEST(test_EXA1_skip_if_key_not_pressed);
    RUN_TEST(test_FX07_get_delay_timer);
    RUN_TEST(test_FX15_set_delay_timer);
    RUN_TEST(test_FX18_set_sound_timer);
    RUN_TEST(test_FX1E_add_I);
    RUN_TEST(test_FX29_font_sprite_addr);
    RUN_TEST(test_FX33_bcd_conversion);
    RUN_TEST(test_FX55_store_registers);
    RUN_TEST(test_FX65_load_registers);
    RUN_TEST(test_EXA1_dont_skip_if_key_is_pressed);
    RUN_TEST(test_drawgfx_basic_sprite_no_collision);
    RUN_TEST(test_drawgfx_collision_flag);
    RUN_TEST(test_drawgfx_horizontal_wrap);

    return UNITY_END();
}
