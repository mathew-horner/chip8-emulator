#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "emulator.h"
#include "io.h"

void print_register_values(CPU *cpu)
{
    printf("I: 0x%x\n", cpu->I);
    printf("dt: 0x%x\n", cpu->dt);
    printf("st: 0x%x\n", cpu->st);
    for (int i = 0; i < 16; i++)
        printf("V%d: 0x%x\n", i, cpu->registers[i]);
}

void repl_loop(Emulator *emulator)
{
    char input[5];
    printf("You are running the Chip-8 Emulator in REPL mode.\n");
    while (1) {
        printf(">>> ");
        scanf("%s", input);
        if (strcmp(input, "exit") == 0) break;
        if (strcmp(input, "registers") == 0) {
            print_register_values(&(emulator->cpu));
        }
        uint16_t instruction = (uint16_t)strtol(input, NULL, 16);
        execute_instruction(emulator, instruction);
        render_frame(&(emulator->display));
    }
}

void debugger_loop(Emulator *emulator)
{
    char input[256];
    printf("You are running the Chip-8 Emulator in Debug mode.\n");
    while (1) {
        printf(">>> ");
        scanf("%s", input);
        if (strcmp(input, "exit") == 0) break;
        if (strcmp(input, "continue") == 0) {
            printf("Not supported yet.\n");
        } else if (strcmp(input, "step") == 0) {
            execute_next_instruction(emulator);
        } else if (strcmp(input, "next") == 0) {
            printf("0x%x\n", next_instruction(emulator));
        } else if (strcmp(input, "previous") == 0) {
            printf("0x%x\n", previous_instruction(emulator));
        } else if (strcmp(input, "registers") == 0) {
            print_register_values(&(emulator->cpu));
        }
    }
}

void execution_loop(Emulator *emulator)
{
    while (1) {
        execute_next_instruction(emulator);
        render_frame(&(emulator->display));
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: chip8-emulator <file>\n");
        exit(1);
    }

    Emulator emulator;
    srand(time(0));
    initialize_display(&(emulator.display));

    if (strcmp(argv[1], "--repl") == 0) {
        repl_loop(&emulator);
    } else {
        char *filepath = argv[1];
        load_rom(&(emulator.memory), filepath);
        initialize_cpu(&(emulator.cpu));

        if (argc > 2 && strcmp(argv[2], "--debug") == 0)
            debugger_loop(&emulator);
        else
            execution_loop(&emulator);
    }

    destroy_display(&(emulator.display));
    return 0;
}
