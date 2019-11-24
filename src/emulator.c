#include "emulator.h"

// Executes a single instruction and increments the PC if needed.
void execute_instruction(Emulator *emulator, uint16_t instruction)
{
    if (instruction == 0x00E0) {
        // CLS
        clear_pixels(&(emulator->display));
        increment_pc(&(emulator->cpu));
    } else if (instruction == 0x00EE) {
        // RET
        move_pc(&(emulator->cpu), emulator->cpu.stack[emulator->cpu.sp]);
        emulator->cpu.sp--;
    } else {
        uint8_t left, right;
        left = instruction >> 12;
        right = instruction & 0xF;

        if (left == 1) {
            // JP addr
            move_pc(&(emulator->cpu), instruction & 0xFFF);
        } else if (left == 2) {
            // CALL addr
            emulator->cpu.stack[emulator->cpu.sp] = emulator->cpu.pc;
            emulator->cpu.sp++;
            move_pc(&(emulator->cpu), instruction & 0xFFF);
        } else if (left == 3) {
            // SE Vx, byte
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t byte = instruction & 0xFF;
            if (emulator->cpu.registers[x] == byte) increment_pc(&(emulator->cpu));
            increment_pc(&(emulator->cpu));
        } else if (left == 4) {
            // SNE Vx, byte
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t byte = instruction & 0xFF;
            if (emulator->cpu.registers[x] != byte) increment_pc(&(emulator->cpu));
            increment_pc(&(emulator->cpu));
        } else if (left == 5) {
            // SE Vx, Vy
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t y = (instruction >> 4) & 0xF;
            if (emulator->cpu.registers[x] == emulator->cpu.registers[y]) increment_pc(&(emulator->cpu));
            increment_pc(&(emulator->cpu));
        } else if (left == 6) {
            // LD Vx, byte
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t byte = instruction & 0xFF;
            emulator->cpu.registers[x] = byte;
            increment_pc(&(emulator->cpu));
        } else if (left == 7) {
            // ADD Vx, byte
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t byte = instruction & 0xFF;
            emulator->cpu.registers[x] += byte;
            increment_pc(&(emulator->cpu));
        } else if (left == 8) {
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t y = (instruction >> 4) & 0xF;

            if (right == 0) {
                // LD Vx, Vy
                emulator->cpu.registers[x] = emulator->cpu.registers[y];
            } else if (right == 1) {
                // OR Vx, Vy
                emulator->cpu.registers[x] |= emulator->cpu.registers[y];
            } else if (right == 2) {
                // AND Vx, Vy
                emulator->cpu.registers[x] &= emulator->cpu.registers[y];
            } else if (right == 3) {
                // XOR Vx, Vy
                emulator->cpu.registers[x] ^= emulator->cpu.registers[y];
            } else if (right == 4) {
                // ADD Vx, Vy
                emulator->cpu.vf = 0;
                uint16_t sum = emulator->cpu.registers[x] + emulator->cpu.registers[y];
                if (sum > 255) {
                    sum = 255;
                    emulator->cpu.vf = 1;
                }
                emulator->cpu.registers[x] = sum;
            } else if (right == 5) {
                // SUB Vx, Vy
                emulator->cpu.vf = 0;
                if (emulator->cpu.registers[x] < emulator->cpu.registers[y]) emulator->cpu.vf = 1;
                emulator->cpu.registers[x] -= emulator->cpu.registers[y];
            } else if (right == 6) {
                // SHR Vx {, Vy}
                emulator->cpu.vf = 0;
                if (emulator->cpu.registers[x] & 1) emulator->cpu.vf = 1;
                emulator->cpu.registers[x] /= 2;
            } else if (right == 7) {
                // SUBN Vx, Vy
                emulator->cpu.vf = 0;
                if (emulator->cpu.registers[x] < emulator->cpu.registers[y]) emulator->cpu.vf = 1;
                emulator->cpu.registers[x] = emulator->cpu.registers[y] - emulator->cpu.registers[x];
            } else if (right == 15) {
                // SHL Vx {, Vy}
                emulator->cpu.vf = 0;
                if ((emulator->cpu.registers[x] >> 7) & 1) emulator->cpu.vf = 1;
                emulator->cpu.registers[x] *= 2;
            }

            increment_pc(&(emulator->cpu));
        } else if (left == 9) {
            // SNE Vx, Vy
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t y = (instruction >> 4) & 0xF;
            if (emulator->cpu.registers[x] != emulator->cpu.registers[y]) increment_pc(&(emulator->cpu));
            increment_pc(&(emulator->cpu));
        } else if (left == 10) {
            // LD I, addr
            emulator->cpu.I = instruction & 0xFFF;
            increment_pc(&(emulator->cpu));
        } else if (left == 11) {
            // JP V0, addr
            move_pc(&(emulator->cpu), emulator->cpu.registers[0] + (instruction & 0xFFF));
        } else if (left == 12) {
            // RND Vx, byte
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t byte = instruction & 0xFF;
            uint8_t random = rand() % 256;
            emulator->cpu.registers[x] = random & byte;
            increment_pc(&(emulator->cpu));
        } else if (left == 15) {
            uint8_t right_two = instruction & 0xFF;
            uint8_t x = (instruction >> 8) & 0xF;
            if (right_two == 0x07) {
                // LD Vx, DT
                emulator->cpu.registers[x] = emulator->cpu.dt;
            } else if (right_two == 0x15) {
                // LD DT, Vx
                emulator->cpu.dt = emulator->cpu.registers[x];
            } else if (right_two == 0x18) {
                // LD ST, Vx
                emulator->cpu.st = emulator->cpu.registers[x];
            } else if (right_two == 0x1E) {
                // ADD I, Vx
                emulator->cpu.I += emulator->cpu.registers[x];
            } else if (right_two == 0x33) {
                // LD B, Vx
                uint8_t x = (instruction >> 8) & 0xF;
                uint8_t temp = emulator->cpu.registers[x];
                for (int i = 2; i >= 0; i--) {
                    emulator->memory.values[emulator->cpu.I + i] = temp % 10;
                    temp /= 10;
                }
            } else if (right_two == 0x55) {
                // LD [I], Vx
                for (int i = 0; i < x; i++)
                    emulator->memory.values[emulator->cpu.I + i] = emulator->cpu.registers[i];
            } else if (right_two == 0x65) {
                // LD Vx, [I]
                for (int i = 0; i < x; i++)
                    emulator->cpu.registers[i] = emulator->memory.values[emulator->cpu.I + i];
            }
            increment_pc(&(emulator->cpu));
        } else {
            increment_pc(&(emulator->cpu));
        }
    }
}

// Executes the instruction at the location of the PC.
void execute_next_instruction(Emulator *emulator)
{
    uint16_t instruction = next_instruction(emulator);
    execute_instruction(emulator, instruction);
}

// Returns the instruction at the location of the PC.
uint16_t next_instruction(Emulator *emulator)
{
    return (emulator->memory.values[emulator->cpu.pc] << 8) | emulator->memory.values[emulator->cpu.pc + 1];
}

// Returns the instruction at the previous PC location.
uint16_t previous_instruction(Emulator *emulator)
{
    if (emulator->cpu.previous_pc == 0)
        return 0;
    return (emulator->memory.values[emulator->cpu.previous_pc] << 8) | emulator->memory.values[emulator->cpu.previous_pc + 1];
}
