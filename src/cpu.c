#include <stdlib.h>
#include "cpu.h"
#include "memory.h"
#include "graphics.h"

// Moves the PC to the next instruction and sets the previous location of the PC.
void increment_pc()
{
    previous_pc = pc;
    pc += 2;
}

// Moves the PC to the given address and sets the previous location of the PC.
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
    dt = 0;
    st = 0;
    vf = 0;

    for (int i = 0; i < 16; i++) {
        registers[i] = 0;
        stack[i] = 0;
    }
}

// Executes a single instruction and increments the PC if needed.
void execute_instruction(uint16_t instruction)
{
    if (instruction == 0x00E0) {
        // CLS
        clear_pixels();
        increment_pc();
    } else if (instruction == 0x00EE) {
        // RET
        move_pc(stack[sp]);
        sp--;
    } else {
        uint8_t left, right;
        left = instruction >> 12;
        right = instruction & 0xF;

        if (left == 1) {
            // JP addr
            move_pc(instruction & 0xFFF);
        } else if (left == 2) {
            // CALL addr
            stack[sp] = pc;
            sp++;
            move_pc(instruction & 0xFFF);
        } else if (left == 3) {
            // SE Vx, byte
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t byte = instruction & 0xFF;
            if (registers[x] == byte) increment_pc();
            increment_pc();
        } else if (left == 4) {
            // SNE Vx, byte
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t byte = instruction & 0xFF;
            if (registers[x] != byte) increment_pc();
            increment_pc();
        } else if (left == 5) {
            // SE Vx, Vy
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t y = (instruction >> 4) & 0xF;
            if (registers[x] == registers[y]) increment_pc();
            increment_pc();
        } else if (left == 6) {
            // LD Vx, byte
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t byte = instruction & 0xFF;
            registers[x] = byte;
            increment_pc();
        } else if (left == 7) {
            // ADD Vx, byte
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t byte = instruction & 0xFF;
            registers[x] += byte;
            increment_pc();
        } else if (left == 8) {
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t y = (instruction >> 4) & 0xF;

            if (right == 0) {
                // LD Vx, Vy
                registers[x] = registers[y];
            } else if (right == 1) {
                // OR Vx, Vy
                registers[x] |= registers[y];
            } else if (right == 2) {
                // AND Vx, Vy
                registers[x] &= registers[y];
            } else if (right == 3) {
                // XOR Vx, Vy
                registers[x] ^= registers[y];
            } else if (right == 4) {
                // ADD Vx, Vy
                vf = 0;
                uint16_t sum = registers[x] + registers[y];
                if (sum > 255) {
                    sum = 255;
                    vf = 1;
                }
                registers[x] = sum;
            } else if (right == 5) {
                // SUB Vx, Vy
                vf = 0;
                if (registers[x] < registers[y]) vf = 1;
                registers[x] -= registers[y];
            } else if (right == 6) {
                // SHR Vx {, Vy}
                vf = 0;
                if (registers[x] & 1) vf = 1;
                registers[x] /= 2;
            } else if (right == 7) {
                // SUBN Vx, Vy
                vf = 0;
                if (registers[x] < registers[y]) vf = 1;
                registers[x] = registers[y] - registers[x];
            } else if (right == 15) {
                // SHL Vx {, Vy}
                vf = 0;
                if ((registers[x] >> 7) & 1) vf = 1;
                registers[x] *= 2;
            }

            increment_pc();
        } else if (left == 9) {
            // SNE Vx, Vy
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t y = (instruction >> 4) & 0xF;
            if (registers[x] != registers[y]) increment_pc();
            increment_pc();
        } else if (left == 10) {
            // LD I, addr
            I = instruction & 0xFFF;
            increment_pc();
        } else if (left == 11) {
            // JP V0, addr
            move_pc(registers[0] + (instruction & 0xFFF));
        } else if (left == 12) {
            // RND Vx, byte
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t byte = instruction & 0xFF;
            uint8_t random = rand() % 256;
            registers[x] = random & byte;
            increment_pc();
        } else if (left == 15) {
            uint8_t right_two = instruction & 0xFF;
            uint8_t x = (instruction >> 8) & 0xF;
            if (right_two == 0x07) {
                // LD Vx, DT
                registers[x] = dt;
            } else if (right_two == 0x15) {
                // LD DT, Vx
                dt = registers[x];
            } else if (right_two == 0x18) {
                // LD ST, Vx
                st = registers[x];
            } else if (right_two == 0x1E) {
                // ADD I, Vx
                I += registers[x];
            } else if (right_two == 0x33) {
                // LD B, Vx
                uint8_t x = (instruction >> 8) & 0xF;
                uint8_t temp = registers[x];
                for (int i = 2; i >= 0; i--) {
                    memory[I + i] = temp % 10;
                    temp /= 10;
                }
            } else if (right_two == 0x55) {
                // LD [I], Vx
                for (int i = 0; i < x; i++)
                    memory[I + i] = registers[i];
            } else if (right_two == 0x65) {
                // LD Vx, [I]
                for (int i = 0; i < x; i++)
                    registers[i] = memory[I + i];
            }
            increment_pc();
        } else {
            increment_pc();
        }
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
