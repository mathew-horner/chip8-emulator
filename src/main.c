#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "cpu.h"
#include "io.h"
#include "graphics.h"
#include "memory.h"

void print_register_values()
{
    printf("I: 0x%x\n", get_I());
    printf("dt: 0x%x\n", get_dt());
    printf("st: 0x%x\n", get_st());
    for (int i = 0; i < 16; i++)
        printf("V%d: 0x%x\n", i, get_register(i));
}

void repl_loop()
{
    char input[5];
    printf("You are running the Chip-8 Emulator in REPL mode.\n");
    while (1) {
        printf(">>> ");
        scanf("%s", input);
        if (strcmp(input, "exit") == 0) break;
        if (strcmp(input, "registers") == 0) {
            print_register_values();
        }
        uint16_t instruction = (uint16_t)strtol(input, NULL, 16);
        execute_instruction(instruction);
        render_frame();
    }
}

void debugger_loop()
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
            execute_next_instruction();
        } else if (strcmp(input, "next") == 0) {
            printf("0x%x\n", next_instruction());
        } else if (strcmp(input, "previous") == 0) {
            printf("0x%x\n", previous_instruction());
        } else if (strcmp(input, "registers") == 0) {
            print_register_values();
        }
    }
}

void execution_loop()
{
    while (1) {
        execute_next_instruction();
        render_frame();
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: chip8-emulator <file>\n");
        exit(1);
    }

    srand(time(0));
    initialize_renderer();

    if (strcmp(argv[1], "--repl") == 0) {
        repl_loop();
    } else {
        char *filepath = argv[1];
        load_rom(filepath);
        initialize_cpu();

        if (argc > 2 && strcmp(argv[2], "--debug") == 0)
            debugger_loop();
        else
            execution_loop();
    }

    destroy_renderer();
    return 0;
}
