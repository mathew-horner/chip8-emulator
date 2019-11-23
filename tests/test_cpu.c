#include <criterion/criterion.h>
#include "../src/cpu.h"
#include "../src/memory.h"

Test(cpu, increment_pc_should_increment_the_pc_and_set_the_previous_pc)
{
    initialize_cpu();
    increment_pc();
    cr_assert(previous_pc == PROGRAM_OFFSET);
    cr_assert(pc == PROGRAM_OFFSET + 2);
}

Test(cpu, move_pc_should_set_the_pc_and_set_the_previous_pc)
{
    initialize_cpu();
    uint16_t address = 1024;
    move_pc(address);
    cr_assert(previous_pc == PROGRAM_OFFSET);
    cr_assert(pc == address);
}