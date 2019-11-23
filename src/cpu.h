#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

uint16_t pc;
uint16_t previous_pc;
uint16_t stack[16];
uint8_t sp;
uint8_t registers[16];
uint16_t I;
bool vf;
uint8_t dt, st;

void increment_pc();
void move_pc(uint16_t address);
void initialize_cpu();
void execute_instruction(uint16_t instruction);
void execute_next_instruction();
uint16_t next_instruction();
uint16_t previous_instruction();

#endif
