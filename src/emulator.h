#ifndef EMULATOR_H
#define EMULATOR_H

#include "cpu.h"
#include "display.h"
#include "memory.h"
#include "time_travel.h"

typedef struct emulator_t {
    CPU cpu;
    Display display;
    uint8_t memory[MEMORY_SIZE];
    
    bool key_state[16];
    bool waiting_for_key;
    int key_register;

    bool record_execution;
    ExecutedInstruction *execution_record_ptr;
} Emulator;

void initialize_emulator(Emulator *emulator);
void initialize_emulator_no_display(Emulator *emulator);
void execute_instruction(Emulator *emulator, uint16_t instruction);
void execute_next_instruction(Emulator *emulator);
uint16_t next_instruction(Emulator *emulator);
uint16_t previous_instruction(Emulator *emulator);

#endif