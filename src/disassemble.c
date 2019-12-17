#include <stdio.h>
#include <stdlib.h>
#include "disassemble.h"

// Returns a CHIP-8 assembly representation of the provided machine code instruction.
// NOTE: This function returns a heap-allocated string, be sure to free it after use.
char *disassemble_instruction(uint16_t instruction)
{
    if (instruction == 0x00E0)
        return "CLS";
    else if (instruction == 0x00EE)
        return "RET";

    int left = instruction >> 12;
    char *assembly = (char *)malloc(1024 * sizeof(char));

    if (left == 1)
        sprintf(assembly, "JP 0x%x", instruction & 0xFFF);
    else if (left == 2)
        sprintf(assembly, "CALL 0x%x", instruction & 0xFFF);
    else if (left == 3)
        sprintf(assembly, "SE V%d, 0x%x", (instruction >> 8) & 0xF, instruction & 0xFF);
    else if (left == 4)
        sprintf(assembly, "SNE V%d, 0x%x", (instruction >> 8) & 0xF, instruction & 0xFF);
    else if (left == 5)
        sprintf(assembly, "SE V%d, V%d", (instruction >> 8) & 0xF, (instruction >> 4) & 0xF);
    else if (left == 6)
        sprintf(assembly, "LD V%d, 0x%x", (instruction >> 8) & 0xF, instruction & 0xFF);
    else if (left == 7)
        sprintf(assembly, "ADD V%d, 0x%x", (instruction >> 8) & 0xF, instruction & 0xFF);
    else if (left == 8) {
        int right = instruction & 0xF;
        if (right == 0)
            sprintf(assembly, "LD V%d, V%d", (instruction >> 8) & 0xF, (instruction >> 4) & 0xF);
        else if (right == 1)
            sprintf(assembly, "OR V%d, V%d", (instruction >> 8) & 0xF, (instruction >> 4) & 0xF);
        else if (right == 2)
            sprintf(assembly, "AND V%d, V%d", (instruction >> 8) & 0xF, (instruction >> 4) & 0xF);
        else if (right == 3)
            sprintf(assembly, "XOR V%d, V%d", (instruction >> 8) & 0xF, (instruction >> 4) & 0xF);
        else if (right == 4)
            sprintf(assembly, "ADD V%d, V%d", (instruction >> 8) & 0xF, (instruction >> 4) & 0xF);
        else if (right == 5)
            sprintf(assembly, "SUB V%d, V%d", (instruction >> 8) & 0xF, (instruction >> 4) & 0xF);
        else if (right == 6)
            sprintf(assembly, "SHR V%d {, V%d}", (instruction >> 8) & 0xF, (instruction >> 4) & 0xF);
        else if (right == 7)
            sprintf(assembly, "SUBN V%d {, V%d}", (instruction >> 8) & 0xF, (instruction >> 4) & 0xF);
        else if (right == 15)
            sprintf(assembly, "SHL V%d {, V%d}", (instruction >> 8) & 0xF, (instruction >> 4) & 0xF);
    } else if (left == 9)
        sprintf(assembly, "SNE V%d, V%d", (instruction >> 8) & 0xF, (instruction >> 4) & 0xF);
    else if (left == 10)
        sprintf(assembly, "LD I, 0x%x", instruction & 0xFFF);
    else if (left == 11)
        sprintf(assembly, "JP V0, 0x%x", instruction & 0xFFF);
    else if (left == 12)
        sprintf(assembly, "RND V%d, 0x%x", (instruction >> 8) & 0xF, instruction & 0xFF);
    else if (left == 13)
        sprintf(assembly, "DRW V%d, V%d, 0x%x", (instruction >> 8) & 0xF, (instruction >> 4) & 0xF, instruction & 0xF);
    else if (left == 14) {
        int right_two = instruction & 0xFF;
        if (right_two == 0x9E)
            sprintf(assembly, "SKP V%d", (instruction >> 8) & 0xF);
        else if (right_two == 0xA1)
            sprintf(assembly, "SKNP V%d", (instruction >> 8) & 0xF);
    } else if (left == 15) {
        int right_two = instruction & 0xFF;
        if (right_two == 0x07)
            sprintf(assembly, "LD V%d, DT", (instruction >> 8) & 0xF);
        else if (right_two == 0x0A)
            sprintf(assembly, "LD V%d, K", (instruction >> 8) & 0xF);
        else if (right_two == 0x15)
            sprintf(assembly, "LD DT, V%d", (instruction >> 8) & 0xF);
        else if (right_two == 0x18)
            sprintf(assembly, "LD ST, V%d", (instruction >> 8) & 0xF);
        else if (right_two == 0x1E)
            sprintf(assembly, "ADD I, V%d", (instruction >> 8) & 0xF);
        else if (right_two == 0x29)
            sprintf(assembly, "LD F, V%d", (instruction >> 8) & 0xF);
        else if (right_two == 0x33)
            sprintf(assembly, "LD B, V%d", (instruction >> 8) & 0xF);
        else if (right_two == 0x55)
            sprintf(assembly, "LD [I], V%d", (instruction >> 8) & 0xF);
        else if (right_two == 0x65)
            sprintf(assembly, "LD V%d, [I]", (instruction >> 8) & 0xF);
    }

    return assembly;
}
