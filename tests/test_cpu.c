#include <criterion/criterion.h>
#include "../src/cpu.h"
#include "../src/memory.h"

Test(cpu, increment_pc_should_increment_the_pc_and_set_the_previous_pc)
{
    initialize_cpu();
    increment_pc();
    cr_assert(get_previous_pc() == PROGRAM_OFFSET);
    cr_assert(get_pc() == PROGRAM_OFFSET + 2);
}

Test(cpu, move_pc_should_set_the_pc_and_set_the_previous_pc)
{
    initialize_cpu();
    uint16_t address = 1024;
    move_pc(address);
    cr_assert(get_previous_pc() == PROGRAM_OFFSET);
    cr_assert(get_pc() == address);
}

Test(cpu, next_instruction_should_return_the_two_bytes_at_the_pc)
{
    initialize_cpu();
    memory[PROGRAM_OFFSET] = 0xFA;
    memory[PROGRAM_OFFSET + 1] = 0xBB;
    uint16_t next = next_instruction();
    cr_assert(next == 0xFABB);
}

Test(cpu, previous_instruction_should_return_the_two_bytes_at_the_previous_pc)
{
    initialize_cpu();
    memory[PROGRAM_OFFSET] = 0xFA;
    memory[PROGRAM_OFFSET + 1] = 0xBB;
    increment_pc();
    uint16_t previous = previous_instruction();
    cr_assert(previous == 0xFABB);
}
