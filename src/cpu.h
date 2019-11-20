#ifndef CPU_H
#define CPU_H

#define PC_STEP 2

#include <stdint.h>

void execute_instruction(uint16_t instruction);
void execute_next_instruction();
void reset_pc();

#endif
