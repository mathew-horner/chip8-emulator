#include <criterion/criterion.h>
#include "../src/cpu.h"
#include "../src/memory.h"

Test(cpu, increment_pc_should_increment_the_pc_and_set_the_previous_pc)
{
    CPU cpu;
    initialize_cpu(&cpu);
    increment_pc(&cpu);
    cr_assert(cpu.previous_pc == PROGRAM_OFFSET);
    cr_assert(cpu.pc == PROGRAM_OFFSET + 2);
}

Test(cpu, move_pc_should_set_the_pc_and_set_the_previous_pc)
{
    CPU cpu;
    initialize_cpu(&cpu);
    uint16_t address = 1024;
    move_pc(&cpu, address);
    cr_assert(cpu.previous_pc == PROGRAM_OFFSET);
    cr_assert(cpu.pc == address);
}
