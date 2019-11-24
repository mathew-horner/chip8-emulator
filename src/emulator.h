#ifndef EMULATOR_H
#define EMULATOR_H

#include "cpu.h"
#include "display.h"
#include "memory.h"

typedef struct emulator_t {
    CPU cpu;
    Display display;
    Memory memory;
} Emulator;

void execute_instruction(Emulator *emulator, uint16_t instruction);
void execute_next_instruction(Emulator *emulator);
uint16_t next_instruction(Emulator *emulator);
uint16_t previous_instruction(Emulator *emulator);

#endif