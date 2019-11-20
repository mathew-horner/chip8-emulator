#include "cpu.h"
#include "memory.h"
#include "graphics.h"

uint16_t pc;

// Executes a single instruction and increments the PC if needed(?)
void execute_instruction(uint16_t instruction)
{
    if (instruction == 0x00E0) {
        clear_pixels();
        pc += PC_STEP;
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
    uint16_t instruction = (memory[pc] << 8) | memory[pc + 1];
    execute_instruction(instruction);
}

void reset_pc()
{
    pc = PROGRAM_OFFSET;
}
