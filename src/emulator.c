#include "emulator.h"

#define HEX_SPRITE_SIZE 80

uint8_t hex_sprites[HEX_SPRITE_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

int get_pressed_key(Emulator *emulator)
{
    int key_pressed = -1;
    for (int i = 0; i < 16; i++) {
        if (emulator->key_state[i]) {
            key_pressed = i;
            break;
        }
    }
    return key_pressed;
}

void load_hex_sprites(Emulator *emulator)
{
    for (int i = 0; i < HEX_SPRITE_SIZE; i++)
        emulator->memory[i] = hex_sprites[i];
}

// Sets the emulator to a state where it is ready to have a program loaded and begin execution.
// Does not initialize the display element, for testing purposes.
void initialize_emulator_no_display(Emulator *emulator)
{
    for (int i = 0; i < MEMORY_SIZE; i++)
        emulator->memory[i] = 0;
    initialize_cpu(&(emulator->cpu));
    emulator->waiting_for_key = false;
    emulator->key_register = -1;
    for (int i = 0; i < 16; i++)
        emulator->key_state[i] = false;
    load_hex_sprites(emulator);
}

// Sets the emulator to a state where it is ready to have a program loaded and begin execution.
void initialize_emulator(Emulator *emulator)
{
    initialize_emulator_no_display(emulator);
    initialize_display(&(emulator->display));
}

void add_execution_record(Emulator *emulator, InstructionType type, void *data)
{
    ExecutedInstruction *instruction = (ExecutedInstruction*)malloc(sizeof(ExecutedInstruction));
    instruction->type = type;
    instruction->data = data;

    if (emulator->execution_record_ptr != NULL)
        instruction->previous = emulator->execution_record_ptr;

    emulator->execution_record_ptr = instruction;
}

// Executes a single instruction and increments the PC if needed.
void execute_instruction(Emulator *emulator, uint16_t instruction)
{
    if (emulator->waiting_for_key) {
        int key_pressed = get_pressed_key(emulator);
        if (key_pressed == -1) return;
        emulator->cpu.registers[emulator->key_register] = key_pressed;
        emulator->waiting_for_key = false;
        emulator->key_register = -1;
    }

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
            if (emulator->record_execution) {
                int *pc = (int *)malloc(sizeof(int));
                *pc = emulator->cpu.pc;
                add_execution_record(emulator, JP_ADDR, (void*)pc);
            }
            move_pc(&(emulator->cpu), instruction & 0xFFF);
        } else if (left == 2) {
            // CALL addr
            emulator->cpu.sp++;
            emulator->cpu.stack[emulator->cpu.sp] = emulator->cpu.pc + 2;
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
                emulator->cpu.registers[15] = 0;
                int sum = emulator->cpu.registers[x] + emulator->cpu.registers[y];
                if (sum > 255) emulator->cpu.registers[15] = 1;
                emulator->cpu.registers[x] = (uint8_t)sum;
            } else if (right == 5) {
                // SUB Vx, Vy
                emulator->cpu.registers[15] = 0;
                if (emulator->cpu.registers[x] > emulator->cpu.registers[y]) emulator->cpu.registers[15] = 1;
                emulator->cpu.registers[x] -= emulator->cpu.registers[y];
            } else if (right == 6) {
                // SHR Vx {, Vy}
                emulator->cpu.registers[15] = 0;
                if (emulator->cpu.registers[x] & 1) emulator->cpu.registers[15] = 1;
                emulator->cpu.registers[x] = emulator->cpu.registers[x] >> 1;
            } else if (right == 7) {
                // SUBN Vx, Vy
                emulator->cpu.registers[15] = 0;
                if (emulator->cpu.registers[x] < emulator->cpu.registers[y]) emulator->cpu.registers[15] = 1;
                emulator->cpu.registers[x] = emulator->cpu.registers[y] - emulator->cpu.registers[x];
            } else if (right == 14) {
                // SHL Vx {, Vy}
                emulator->cpu.registers[15] = 0;
                if ((emulator->cpu.registers[x] >> 7) & 1) emulator->cpu.registers[15] = 1;
                emulator->cpu.registers[x] = emulator->cpu.registers[x] << 1;
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
        } else if (left == 13) {
            // DRW Vx, Vy, nibble
            emulator->cpu.registers[15] = 0;
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t y = (instruction >> 4) & 0xF;
            uint8_t n = (instruction) & 0xF;
            
            int coord_x = emulator->cpu.registers[x];
            int coord_y = emulator->cpu.registers[y];

            for (int i = 0; i < n; i++) {
                uint8_t data = emulator->memory[emulator->cpu.I + i];
                for (int j = 0; j < 8; j++) {
                    int row = (coord_y % DISPLAY_HEIGHT) + i;
                    int col = (coord_x % DISPLAY_WIDTH) + (7 - j);
                    int previous = emulator->display.pixels[row][col];
                    int new = previous ^ data & 0x1;
                    emulator->display.pixels[row][col] = new;
                    if (previous == 1 && new == 0)
                        emulator->cpu.registers[15] = 1;
                    data >>= 1;
                }
            }

            increment_pc(&(emulator->cpu));
        } else if (left == 14) {
            uint8_t right_two = instruction & 0xFF;
            uint8_t x = (instruction >> 8) & 0xF;
            if (right_two == 0x9E) {
                // SKP Vx
                if (emulator->key_state[emulator->cpu.registers[x]])
                    increment_pc(&(emulator->cpu));
            } else if (right_two == 0xA1) {
                // SKNP Vx
                if (!emulator->key_state[emulator->cpu.registers[x]])
                    increment_pc(&(emulator->cpu));
            }
            increment_pc(&(emulator->cpu));
        } else if (left == 15) {
            uint8_t right_two = instruction & 0xFF;
            uint8_t x = (instruction >> 8) & 0xF;
            if (right_two == 0x07) {
                // LD Vx, DT
                emulator->cpu.registers[x] = emulator->cpu.dt;
            } else if (right_two == 0x0A) {
                // LD Vx, K
                int key_pressed = get_pressed_key(emulator);
                if (key_pressed != -1) {
                    emulator->cpu.registers[x] = key_pressed;
                } else {
                    emulator->waiting_for_key = true;
                    emulator->key_register = x;
                }
            } else if (right_two == 0x15) {
                // LD DT, Vx
                emulator->cpu.dt = emulator->cpu.registers[x];
            } else if (right_two == 0x18) {
                // LD ST, Vx
                emulator->cpu.st = emulator->cpu.registers[x];
            } else if (right_two == 0x1E) {
                // ADD I, Vx
                emulator->cpu.I += emulator->cpu.registers[x];
            } else if (right_two == 0x29) {
                // LD F, Vx
                emulator->cpu.I = emulator->cpu.registers[x] * 5;
            } else if (right_two == 0x33) {
                // LD B, Vx
                uint8_t x = (instruction >> 8) & 0xF;
                uint8_t temp = emulator->cpu.registers[x];
                for (int i = 2; i >= 0; i--) {
                    emulator->memory[emulator->cpu.I + i] = temp % 10;
                    temp /= 10;
                }
            } else if (right_two == 0x55) {
                // LD [I], Vx
                for (int i = 0; i <= x; i++)
                    emulator->memory[emulator->cpu.I + i] = emulator->cpu.registers[i];
            } else if (right_two == 0x65) {
                // LD Vx, [I]
                for (int i = 0; i <= x; i++)
                    emulator->cpu.registers[i] = emulator->memory[emulator->cpu.I + i];
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
    return (emulator->memory[emulator->cpu.pc] << 8) | emulator->memory[emulator->cpu.pc + 1];
}

// Returns the instruction at the previous PC location.
uint16_t previous_instruction(Emulator *emulator)
{
    if (emulator->cpu.previous_pc == 0)
        return 0;
    return (emulator->memory[emulator->cpu.previous_pc] << 8) | emulator->memory[emulator->cpu.previous_pc + 1];
}
