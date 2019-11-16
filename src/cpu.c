#include "cpu.h"
#include "memory.h"

uint16_t pc;

// Executes a single instruction and increments the PC if needed(?)
void execute_instruction(uint16_t instruction)
{
    if (instruction == 0x00E0) {
        // TODO: Implement CLS.
    } else if (instruction == 0x00EE) {
        // TODO: Implement RET.
    } else {
        uint8_t left, right;
        left = instruction >> 12;
        right = instruction & 0xF;
        // TODO: Implement other opcodes.
    }
}

// Begins executing the program loaded into memory.
void start_cpu()
{
    pc = PROGRAM_OFFSET;
    while (1) {
        uint16_t instruction = (memory[pc] << 8) | memory[pc + 1];
        execute_instruction(instruction);
    }
}
