#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

void increment_pc();
void move_pc(uint16_t address);
void initialize_cpu();
void execute_instruction(uint16_t instruction);
void execute_next_instruction();
uint16_t next_instruction();
uint16_t previous_instruction();

uint16_t get_pc();
uint16_t get_previous_pc();
uint8_t get_sp();
uint16_t get_I();
uint8_t get_dt();
uint8_t get_st();
uint16_t get_stack(int index);
uint8_t get_register(int index);

#endif
