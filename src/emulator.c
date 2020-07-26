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

//void (*instruction_functions) (Emulator *emulator, uint16_t instruction);
void (*undo_functions[34]) (Emulator *emulator, void *data) = {
    undo_CLS,
    NULL,
    NULL,
    undo_CALL_ADDR,
    NULL, 
    NULL,
    NULL,
    undo_LD_VX_BYTE,
    undo_ADD_VX_BYTE,
    undo_LD_VX_VY,
    undo_OR_VX_VY,
    undo_AND_VX_VY,
    undo_XOR_VX_VY,
    undo_ADD_VX_VY,
    undo_SUB_VX_VY,
    undo_SHR_VX_VY,
    undo_SUBN_VX_VY,
    undo_SHL_VX_VY,
    undo_SNE_VX_VY,
    undo_LD_I_ADDR,
    undo_JP_V0_ADDR,
    undo_RND_VX_BYTE,
    undo_DRW_VX_VY_NIBBLE,
    undo_SKP_VX,
    undo_SKNP_VX,
    undo_LD_VX_DT,
    undo_LD_VX_K,
    undo_LD_DT_VX,
    undo_LD_ST_VX,
    undo_ADD_I_VX,
    undo_LD_F_VX,
    undo_LD_B_VX,
    undo_LD_I_VX,
    undo_LD_VX_I
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
    emulator->record_execution = false;
    emulator->execution_record_ptr = NULL;
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
    instruction->address = emulator->cpu.pc;
    instruction->previous = (emulator->execution_record_ptr != NULL) ? emulator->execution_record_ptr : NULL;
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

    void (*fp) (Emulator *emulator, uint16_t instruction) = NULL;
    
    if (instruction == 0x00E0)
        fp = do_CLS;
    else if (instruction == 0x00EE)
        fp = do_RET;
    else {
        uint8_t left, right;
        left = instruction >> 12;
        right = instruction & 0xF;

        if (left == 1)
            fp = do_JP_ADDR;
        else if (left == 2)
            fp = do_CALL_ADDR;
        else if (left == 3)
            fp = do_SE_VX_BYTE;
        else if (left == 4)
            fp = do_SNE_VX_BYTE;
        else if (left == 5)
            fp = do_SE_VX_VY;
        else if (left == 6)
            fp = do_LD_VX_BYTE;
        else if (left == 7)
            fp = do_ADD_VX_BYTE;
        else if (left == 8) {
            if (right == 0)
                fp = do_LD_VX_VY;
            else if (right == 1)
                fp = do_OR_VX_VY;
            else if (right == 2)
                fp = do_AND_VX_VY;
            else if (right == 3)
                fp = do_XOR_VX_VY;
            else if (right == 4)
                fp = do_ADD_VX_VY;
            else if (right == 5)
                fp = do_SUB_VX_VY;
            else if (right == 6)
                fp = do_SHR_VX_VY;
            else if (right == 7)
                fp = do_SUBN_VX_VY;
            else if (right == 14)
                fp = do_SHL_VX_VY;
        } else if (left == 9)
            fp = do_SNE_VX_VY;
        else if (left == 10)
            fp = do_LD_I_ADDR;
        else if (left == 11)
            fp = do_JP_V0_ADDR;
        else if (left == 12)
            fp = do_RND_VX_BYTE;
        else if (left == 13)
            fp = do_DRW_VX_VY_NIBBLE;
        else if (left == 14) {
            uint8_t right_two = instruction & 0xFF;
            if (right_two == 0x9E)
                fp = do_SKP_VX;
            else if (right_two == 0xA1)
                fp = do_SKNP_VX;
        } else if (left == 15) {
            uint8_t right_two = instruction & 0xFF;
            if (right_two == 0x07)
                fp = do_LD_VX_DT;
            else if (right_two == 0x0A)
                fp = do_LD_VX_K;
            else if (right_two == 0x15)
                fp = do_LD_DT_VX;
            else if (right_two == 0x18)
                fp = do_LD_ST_VX;
            else if (right_two == 0x1E)
                fp = do_ADD_I_VX;
            else if (right_two == 0x29)
                fp = do_LD_F_VX;
            else if (right_two == 0x33)
                fp = do_LD_B_VX;
            else if (right_two == 0x55)
                fp = do_LD_I_VX;
            else if (right_two == 0x65)
                fp = do_LD_VX_I;
        }
    }

    if (fp != NULL)
        fp(emulator, instruction);
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

// Calls the undo function for the most recently executed instruction.
int revert_last_instruction(Emulator *emulator)
{
    ExecutedInstruction *instruction = emulator->execution_record_ptr;
    if (instruction == NULL)
        return 1;
   
    if (undo_functions[instruction->type] != NULL)
        undo_functions[instruction->type](emulator, instruction->data);

    emulator->cpu.pc = instruction->address;
    emulator->execution_record_ptr = instruction->previous;

    if (instruction->previous != NULL) {
        emulator->cpu.previous_pc = instruction->previous->address;
    } else {
        emulator->cpu.previous_pc = 0;
    }

    free(instruction);
}


// ----------------------------
//  INSTRUCTION "DO" FUNCTIONS
// ----------------------------

void do_CLS(Emulator *emulator, uint16_t instruction)
{
    clear_pixels(&(emulator->display));
    increment_pc(&(emulator->cpu));
}

void do_RET(Emulator *emulator, uint16_t instruction)
{
    if (emulator->record_execution)
        add_execution_record(emulator, RET, NULL);

    move_pc(&(emulator->cpu), emulator->cpu.stack[emulator->cpu.sp]);
    emulator->cpu.sp--;
}

void do_JP_ADDR(Emulator *emulator, uint16_t instruction)
{
    if (emulator->record_execution)
        add_execution_record(emulator, JP_ADDR, NULL);
    move_pc(&(emulator->cpu), instruction & 0xFFF);
}

void do_CALL_ADDR(Emulator *emulator, uint16_t instruction)
{
    if (emulator->record_execution)
        add_execution_record(emulator, CALL_ADDR, NULL);

    emulator->cpu.sp++;
    emulator->cpu.stack[emulator->cpu.sp] = emulator->cpu.pc + 2;
    move_pc(&(emulator->cpu), instruction & 0xFFF);
}

void do_SE_VX_BYTE(Emulator *emulator, uint16_t instruction)
{
    if (emulator->record_execution)
        add_execution_record(emulator, SE_VX_BYTE, NULL);

    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t byte = instruction & 0xFF;

    if (emulator->cpu.registers[x] == byte)
        increment_pc(&(emulator->cpu));

    increment_pc(&(emulator->cpu));
}

void do_SNE_VX_BYTE(Emulator *emulator, uint16_t instruction)
{
    if (emulator->record_execution)
        add_execution_record(emulator, SE_VX_BYTE, NULL);

    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t byte = instruction & 0xFF;

    if (emulator->cpu.registers[x] != byte)
        increment_pc(&(emulator->cpu));

    increment_pc(&(emulator->cpu));
}

void do_SE_VX_VY(Emulator *emulator, uint16_t instruction)
{
    if (emulator->record_execution)
        add_execution_record(emulator, SE_VX_BYTE, NULL);

    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t y = (instruction >> 4) & 0xF;

    if (emulator->cpu.registers[x] == emulator->cpu.registers[y])
        increment_pc(&(emulator->cpu));

    increment_pc(&(emulator->cpu));
}

void do_LD_VX_BYTE(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t byte = instruction & 0xFF;
    emulator->cpu.registers[x] = byte;
    increment_pc(&(emulator->cpu));
}

void do_ADD_VX_BYTE(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t byte = instruction & 0xFF;
    emulator->cpu.registers[x] += byte;
    increment_pc(&(emulator->cpu));
}

void do_LD_VX_VY(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t y = (instruction >> 4) & 0xF;
    emulator->cpu.registers[x] = emulator->cpu.registers[y];
    increment_pc(&(emulator->cpu));
}

void do_OR_VX_VY(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t y = (instruction >> 4) & 0xF;
    emulator->cpu.registers[x] |= emulator->cpu.registers[y];
    increment_pc(&(emulator->cpu));
}

void do_AND_VX_VY(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t y = (instruction >> 4) & 0xF;
    emulator->cpu.registers[x] &= emulator->cpu.registers[y];
    increment_pc(&(emulator->cpu));
}

void do_XOR_VX_VY(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t y = (instruction >> 4) & 0xF;
    emulator->cpu.registers[x] ^= emulator->cpu.registers[y];
    increment_pc(&(emulator->cpu));
}

void do_ADD_VX_VY(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t y = (instruction >> 4) & 0xF;

    emulator->cpu.registers[15] = 0;
    int sum = emulator->cpu.registers[x] + emulator->cpu.registers[y];
    emulator->cpu.registers[x] = (uint8_t)sum;

    if (sum > 255)
        emulator->cpu.registers[15] = 1;
    
    increment_pc(&(emulator->cpu));
}

void do_SUB_VX_VY(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t y = (instruction >> 4) & 0xF;

    emulator->cpu.registers[15] = 0;
    emulator->cpu.registers[x] -= emulator->cpu.registers[y];

    if (emulator->cpu.registers[x] > emulator->cpu.registers[y])
        emulator->cpu.registers[15] = 1;
    
    increment_pc(&(emulator->cpu));
}

void do_SHR_VX_VY(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t y = (instruction >> 4) & 0xF;

    emulator->cpu.registers[15] = 0;
    emulator->cpu.registers[x] = emulator->cpu.registers[x] >> 1;

    if (emulator->cpu.registers[x] & 1)
        emulator->cpu.registers[15] = 1;
    
    increment_pc(&(emulator->cpu));
}

void do_SUBN_VX_VY(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t y = (instruction >> 4) & 0xF;

    emulator->cpu.registers[15] = 0;
    emulator->cpu.registers[x] = emulator->cpu.registers[y] - emulator->cpu.registers[x];

    if (emulator->cpu.registers[x] < emulator->cpu.registers[y])
        emulator->cpu.registers[15] = 1;
    
    increment_pc(&(emulator->cpu));
}

void do_SHL_VX_VY(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t y = (instruction >> 4) & 0xF;

    emulator->cpu.registers[15] = 0;
    emulator->cpu.registers[x] = emulator->cpu.registers[x] << 1;

    if ((emulator->cpu.registers[x] >> 7) & 1)
        emulator->cpu.registers[15] = 1;
    
    increment_pc(&(emulator->cpu));
}

void do_SNE_VX_VY(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t y = (instruction >> 4) & 0xF;
    
    if (emulator->cpu.registers[x] != emulator->cpu.registers[y])
        increment_pc(&(emulator->cpu));
    
    increment_pc(&(emulator->cpu));
}

void do_LD_I_ADDR(Emulator *emulator, uint16_t instruction)
{
    emulator->cpu.I = instruction & 0xFFF;
    increment_pc(&(emulator->cpu));
}

void do_JP_V0_ADDR(Emulator *emulator, uint16_t instruction)
{
    if (emulator->record_execution)
        add_execution_record(emulator, JP_V0_ADDR, NULL);
    move_pc(&(emulator->cpu), emulator->cpu.registers[0] + (instruction & 0xFFF));
}

void do_RND_VX_BYTE(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t byte = instruction & 0xFF;
    uint8_t random = rand() % 256;
    emulator->cpu.registers[x] = random & byte;
    increment_pc(&(emulator->cpu));
}

void do_DRW_VX_VY_NIBBLE(Emulator *emulator, uint16_t instruction)
{
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
}

void do_SKP_VX(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    if (emulator->key_state[emulator->cpu.registers[x]])
        increment_pc(&(emulator->cpu));

    increment_pc(&(emulator->cpu));
}

void do_SKNP_VX(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    if (!emulator->key_state[emulator->cpu.registers[x]])
        increment_pc(&(emulator->cpu));

    increment_pc(&(emulator->cpu));
}

void do_LD_VX_DT(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    emulator->cpu.registers[x] = emulator->cpu.dt;
    increment_pc(&(emulator->cpu));
}

void do_LD_VX_K(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    int key_pressed = get_pressed_key(emulator);

    if (key_pressed != -1) {
        emulator->cpu.registers[x] = key_pressed;
    } else {
        emulator->waiting_for_key = true;
        emulator->key_register = x;
    }

    increment_pc(&(emulator->cpu));
}

void do_LD_DT_VX(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    emulator->cpu.dt = emulator->cpu.registers[x];
    increment_pc(&(emulator->cpu));
}

void do_LD_ST_VX(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    emulator->cpu.st = emulator->cpu.registers[x];
    increment_pc(&(emulator->cpu));
}

void do_ADD_I_VX(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    emulator->cpu.I += emulator->cpu.registers[x];
    increment_pc(&(emulator->cpu));
}

void do_LD_F_VX(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    emulator->cpu.I = emulator->cpu.registers[x] * 5;
    increment_pc(&(emulator->cpu));
}

void do_LD_B_VX(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t temp = emulator->cpu.registers[x];

    for (int i = 2; i >= 0; i--) {
        emulator->memory[emulator->cpu.I + i] = temp % 10;
        temp /= 10;
    }

    increment_pc(&(emulator->cpu));
}

void do_LD_I_VX(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    for (int i = 0; i <= x; i++)
        emulator->memory[emulator->cpu.I + i] = emulator->cpu.registers[i];

    increment_pc(&(emulator->cpu));
}

void do_LD_VX_I(Emulator *emulator, uint16_t instruction)
{
    uint8_t x = (instruction >> 8) & 0xF;
    for (int i = 0; i <= x; i++)
        emulator->cpu.registers[i] = emulator->memory[emulator->cpu.I + i];

    increment_pc(&(emulator->cpu));
}


// ----------------------------
//  INSTRUCTION UNDO FUNCTIONS
// ----------------------------

void undo_CLS(Emulator *emulator, void *data)
{

}

void undo_CALL_ADDR(Emulator *emulator, void *data)
{
    emulator->cpu.stack[emulator->cpu.sp] = 0;
    emulator->cpu.sp--;
}

void undo_LD_VX_BYTE(Emulator *emulator, void *data)
{

}

void undo_ADD_VX_BYTE(Emulator *emulator, void *data)
{

}

void undo_LD_VX_VY(Emulator *emulator, void *data)
{

}

void undo_OR_VX_VY(Emulator *emulator, void *data)
{

}

void undo_AND_VX_VY(Emulator *emulator, void *data)
{

}

void undo_XOR_VX_VY(Emulator *emulator, void *data)
{

}

void undo_ADD_VX_VY(Emulator *emulator, void *data)
{

}

void undo_SUB_VX_VY(Emulator *emulator, void *data)
{

}

void undo_SHR_VX_VY(Emulator *emulator, void *data)
{

}

void undo_SUBN_VX_VY(Emulator *emulator, void *data)
{

}

void undo_SHL_VX_VY(Emulator *emulator, void *data)
{

}

void undo_SNE_VX_VY(Emulator *emulator, void *data)
{

}

void undo_LD_I_ADDR(Emulator *emulator, void *data)
{

}

void undo_JP_V0_ADDR(Emulator *emulator, void *data)
{

}

void undo_RND_VX_BYTE(Emulator *emulator, void *data)
{

}

void undo_DRW_VX_VY_NIBBLE(Emulator *emulator, void *data)
{

}

void undo_SKP_VX(Emulator *emulator, void *data)
{

}

void undo_SKNP_VX(Emulator *emulator, void *data)
{

}

void undo_LD_VX_DT(Emulator *emulator, void *data)
{

}

void undo_LD_VX_K(Emulator *emulator, void *data)
{

}

void undo_LD_DT_VX(Emulator *emulator, void *data)
{

}

void undo_LD_ST_VX(Emulator *emulator, void *data)
{

}

void undo_ADD_I_VX(Emulator *emulator, void *data)
{

}

void undo_LD_F_VX(Emulator *emulator, void *data)
{

}

void undo_LD_B_VX(Emulator *emulator, void *data)
{

}

void undo_LD_I_VX(Emulator *emulator, void *data)
{

}

void undo_LD_VX_I(Emulator *emulator, void *data)
{

}
