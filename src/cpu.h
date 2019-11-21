#ifndef CPU_H
#define CPU_H

#include <stdint.h>

void initialize_cpu();
void execute_instruction(uint16_t instruction);
void execute_next_instruction();
uint16_t next_instruction();
uint16_t previous_instruction();
uint8_t register_value(uint8_t number);

#endif
