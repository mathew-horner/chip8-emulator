#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cpu.h"
#include "io.h"
#include "graphics.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: chip8-emulator <file>\n");
        exit(1);
    }

    initialize_renderer();

    if (strcmp(argv[1], "--repl") == 0) {
        char input[5];
        printf("You are running the Chip-8 Emulator in REPL mode.\n");
        while (1) {
            printf(">>> ");
            scanf("%s", input);
            if (strcmp(input, "exit") == 0) break;
            uint16_t instruction = (uint16_t)strtol(input, NULL, 16);
            execute_instruction(instruction);
            render_frame();
        }
    } else {
        char *filepath = argv[1];
        load_rom(filepath);
        reset_pc();
        while (1) {
            execute_next_instruction();
            render_frame();
        }
    }

    destroy_renderer();
    return 0;
}
