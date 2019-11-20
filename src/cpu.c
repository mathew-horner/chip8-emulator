#include "cpu.h"
#include "memory.h"
#include "graphics.h"

uint16_t pc;
uint16_t previous_pc;

void increment_pc()
{
    previous_pc = pc;
    pc += 2;
}

void reset_pc()
{
    previous_pc = -1;
    pc = PROGRAM_OFFSET;
}

// Executes a single instruction and increments the PC if needed.
void execute_instruction(uint16_t instruction)
{
    if (instruction == 0x00E0) {
        clear_pixels();
        increment_pc();
    } else if (instruction == 0x00EE) {
        // TODO: Implement RET.
    } else {
        uint8_t left, right;
        left = instruction >> 12;
        right = instruction & 0xF;
        // TODO: Implement other opcodes.
    }
}

// Executes the instruction at the location of the PC.
void execute_next_instruction()
{
    uint16_t instruction = next_instruction();
    execute_instruction(instruction);
}

// Returns the instruction at the location of the PC.
uint16_t next_instruction()
{
    return (memory[pc] << 8) | memory[pc + 1];
}

// Returns the instruction at the previous PC location.
uint16_t previous_instruction()
{
    if (previous_pc == -1)
        return 0;
    return (memory[previous_pc] << 8) | memory[previous_pc + 1];
}
