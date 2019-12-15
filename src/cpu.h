#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

typedef struct cpu_t {
    uint16_t pc;
    uint16_t previous_pc;
    uint16_t stack[16];
    int sp;
    uint8_t registers[16];
    uint16_t I;
    uint8_t dt, st;
} CPU;

void increment_pc(CPU *cpu);
void move_pc(CPU *cpu, uint16_t address);
void initialize_cpu(CPU *cpu);
void decrement_dt(CPU *cpu);

#endif
