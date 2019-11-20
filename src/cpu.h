#ifndef CPU_H
#define CPU_H

#include <stdint.h>

void execute_instruction(uint16_t instruction);
void execute_next_instruction();
uint16_t next_instruction();
uint16_t previous_instruction();

void reset_pc();

#endif
