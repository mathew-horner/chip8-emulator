#include "cpu.h"
#include "memory.h"
#include "graphics.h"

uint16_t pc;
uint16_t previous_pc;
uint16_t stack[16];
uint8_t sp;

void increment_pc()
{
    previous_pc = pc;
    pc += 2;
}

void move_pc(uint16_t address)
{
    previous_pc = pc;
    pc = address;
}

// Sets the CPU to a state where it is ready to begin executing a program.
void initialize_cpu()
{
    previous_pc = 0;
    pc = PROGRAM_OFFSET;
    sp = 0;
    for (int i = 0; i < 16; i++)
        stack[i] = 0;
}

// Executes a single instruction and increments the PC if needed.
void execute_instruction(uint16_t instruction)
{
    if (instruction == 0x00E0) {
        // CLS
        clear_pixels();
        increment_pc();
    } else if (instruction == 0x00EE) {
        // TODO: Implement RET.
    } else {
        uint8_t left, right;
        left = instruction >> 12;
        right = instruction & 0xF;

        if (left == 2) {
            // CALL addr
            stack[sp] = pc;
            sp++;
            move_pc(instruction & 0xFFF);
        }
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
    if (previous_pc == 0)
        return 0;
    return (memory[previous_pc] << 8) | memory[previous_pc + 1];
}
