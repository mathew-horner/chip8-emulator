#include <criterion/criterion.h>
#include "../src/emulator.h"

Test(emulator, test_CLS)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));

    for (int i = 0; i < DISPLAY_HEIGHT; i++)
        for (int j = 0; j < DISPLAY_WIDTH; j++)
            emulator.display.pixels[i][j] = 1;

    execute_instruction(&emulator, 0x00E0);
    
    for (int i = 0; i < DISPLAY_HEIGHT; i++)
        for (int j = 0; j < DISPLAY_WIDTH; j++)
            cr_assert(emulator.display.pixels[i][j] == 0);
    
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_RET)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.sp = 0;
    emulator.cpu.stack[0] = 1024;

    execute_instruction(&emulator, 0x00EE);

    cr_assert(emulator.cpu.sp == -1);
    cr_assert(emulator.cpu.pc == 1024);
}

Test(emulator, test_JP_addr)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));

    execute_instruction(&emulator, 0x1400);

    cr_assert(emulator.cpu.pc == 1024);
}

Test(emulator, test_CALL_addr)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    
    execute_instruction(&emulator, 0x2400);

    cr_assert(emulator.cpu.sp == 0);
    cr_assert(emulator.cpu.stack[0] == PROGRAM_OFFSET);
    cr_assert(emulator.cpu.pc == 1024);
}

Test(emulator, test_SE_Vx_byte_skips_next_instruction_if_equal)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xFF;

    execute_instruction(&emulator, 0x30FF);

    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 4);
}

Test(emulator, test_SE_Vx_byte_does_not_skip_next_instruction_if_not_equal)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xAA;

    execute_instruction(&emulator, 0x30FF);

    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_SNE_Vx_byte_skips_next_instruction_if_not_equal)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xAA;

    execute_instruction(&emulator, 0x40FF);

    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 4);
}

Test(emulator, test_SNE_Vx_byte_does_not_skip_next_instruction_if_equal)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xFF;

    execute_instruction(&emulator, 0x40FF);

    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_SE_Vx_Vy_skips_next_instruction_if_equal)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xFF;
    emulator.cpu.registers[1] = 0xFF;

    execute_instruction(&emulator, 0x5010);

    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 4);
}

Test(emulator, test_SE_Vx_Vy_does_not_skip_next_instruction_if_not_equal)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xFF;
    emulator.cpu.registers[1] = 0xAA;

    execute_instruction(&emulator, 0x5010);

    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_LD_Vx_byte)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));

    execute_instruction(&emulator, 0x60AA);

    cr_assert(emulator.cpu.registers[0] == 0xAA);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_ADD_Vx_byte)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xAA;

    execute_instruction(&emulator, 0x7001);

    cr_assert(emulator.cpu.registers[0] == 0xAB);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_LD_Vx_Vy)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xAA;

    execute_instruction(&emulator, 0x8100);

    cr_assert(emulator.cpu.registers[1] == 0xAA);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_OR_Vx_Vy)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xA1;
    emulator.cpu.registers[1] = 0x2F;

    execute_instruction(&emulator, 0x8101);

    cr_assert(emulator.cpu.registers[1] == 0xAF);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_AND_Vx_Vy)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xA1;
    emulator.cpu.registers[1] = 0x2F;

    execute_instruction(&emulator, 0x8102);

    cr_assert(emulator.cpu.registers[1] == 0x21);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_XOR_Vx_Vy)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xA1;
    emulator.cpu.registers[1] = 0x2F;

    execute_instruction(&emulator, 0x8103);

    cr_assert(emulator.cpu.registers[1] == 0x8E);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_ADD_Vx_Vy)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xA1;
    emulator.cpu.registers[1] = 1;

    execute_instruction(&emulator, 0x8104);

    cr_assert(emulator.cpu.registers[1] == 0xA2);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_SUB_Vx_Vy)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 1;
    emulator.cpu.registers[1] = 0xA1;

    execute_instruction(&emulator, 0x8105);

    cr_assert(emulator.cpu.registers[1] == 0xA0);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_SHR_Vx_Vy)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 4;

    execute_instruction(&emulator, 0x8006);

    cr_assert(emulator.cpu.registers[0] == 2);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_SUBN_Vx_Vy)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 1;
    emulator.cpu.registers[1] = 0xFF;

    execute_instruction(&emulator, 0x8017);

    cr_assert(emulator.cpu.registers[0] == 0xFE);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_SHL_Vx_Vy)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 2;

    execute_instruction(&emulator, 0x800F);

    cr_assert(emulator.cpu.registers[0] == 4);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_SNE_Vx_Vy_skips_next_instruction_if_not_equal)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xFF;
    emulator.cpu.registers[1] = 0xAA;

    execute_instruction(&emulator, 0x9010);

    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 4);
}

Test(emulator, test_SNE_Vx_Vy_does_not_skip_next_instruction_if_equal)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xFF;
    emulator.cpu.registers[1] = 0xFF;

    execute_instruction(&emulator, 0x9010);

    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_LD_I_addr)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));

    execute_instruction(&emulator, 0xA400);

    cr_assert(emulator.cpu.I == 1024);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_JP_V0_addr)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xFF;
    
    execute_instruction(&emulator, 0xB400);

    cr_assert(emulator.cpu.pc == 1279);
}

Test(emulator, test_RND_Vx_byte)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));

    execute_instruction(&emulator, 0xC0FF);

    // Since this operation is non-deterministic, we will only assert on the
    // PC being incremented.
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_LD_Vx_DT)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.dt = 16;

    execute_instruction(&emulator, 0xF007);

    cr_assert(emulator.cpu.registers[0] == 16);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_LD_DT_Vx)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 16;

    execute_instruction(&emulator, 0xF015);

    cr_assert(emulator.cpu.dt == 16);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_LD_ST_Vx)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 16;

    execute_instruction(&emulator, 0xF018);

    cr_assert(emulator.cpu.st == 16);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_ADD_I_Vx)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.I = 1024;
    emulator.cpu.registers[0] = 0xFF;

    execute_instruction(&emulator, 0xF01E);

    cr_assert(emulator.cpu.I == 1279);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_LD_B_Vx_3_digits)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xFF;
    emulator.cpu.I = 1024;

    execute_instruction(&emulator, 0xF033);

    cr_assert(emulator.memory[1024] == 2);
    cr_assert(emulator.memory[1025] == 5);
    cr_assert(emulator.memory[1026] == 5);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_LD_B_Vx_2_digits)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0x10;
    emulator.cpu.I = 1024;

    execute_instruction(&emulator, 0xF033);

    cr_assert(emulator.memory[1024] == 0);
    cr_assert(emulator.memory[1025] == 1);
    cr_assert(emulator.memory[1026] == 6);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_LD_B_Vx_1_digit)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0x3;
    emulator.cpu.I = 1024;

    execute_instruction(&emulator, 0xF033);

    cr_assert(emulator.memory[1024] == 0);
    cr_assert(emulator.memory[1025] == 0);
    cr_assert(emulator.memory[1026] == 3);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_LD_B_Vx_0)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0;
    emulator.cpu.I = 1024;

    execute_instruction(&emulator, 0xF033);

    cr_assert(emulator.memory[1024] == 0);
    cr_assert(emulator.memory[1025] == 0);
    cr_assert(emulator.memory[1026] == 0);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_LD_I_Vx)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.cpu.registers[0] = 0xFF;
    emulator.cpu.registers[1] = 0xAA;
    emulator.cpu.registers[2] = 0x2F;
    emulator.cpu.I = 1024;

    execute_instruction(&emulator, 0xF255);

    cr_assert(emulator.memory[1024] == 0xFF);
    cr_assert(emulator.memory[1025] == 0xAA);
    cr_assert(emulator.memory[1026] == 0x2F);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}

Test(emulator, test_LD_Vx_I)
{
    Emulator emulator;
    initialize_cpu(&(emulator.cpu));
    emulator.memory[1024] = 0xFF;
    emulator.memory[1025] = 0xAA;
    emulator.memory[1026] = 0x2F;
    emulator.cpu.I = 1024;

    execute_instruction(&emulator, 0xF265);

    cr_assert(emulator.cpu.registers[0] == 0xFF);
    cr_assert(emulator.cpu.registers[1] == 0xAA);
    cr_assert(emulator.cpu.registers[2] == 0x2F);
    cr_assert(emulator.cpu.pc == PROGRAM_OFFSET + 2);
}