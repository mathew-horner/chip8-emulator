#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "debugger.h"
#include "emulator.h"
#include "io.h"

void repl_loop(Emulator *emulator)
{
    char input[5];
    printf("You are running the Chip-8 Emulator in REPL mode.\n");
    while (1) {
        printf(">>> ");
        scanf("%s", input);
        uint16_t instruction = (uint16_t)strtol(input, NULL, 16);
        execute_instruction(emulator, instruction);
        render_frame(&(emulator->display));
    }
}

void debugger_loop(Emulator *emulator)
{
    Breakpoints breakpoints;
    breakpoints.address_count = 0;

    bool pass = false;
    char *input;
    size_t n;

    printf("You are running the Chip-8 Emulator in Debug mode.\n");

    while (1) {
        if (!pass || should_break(emulator, &breakpoints)) {
            pass = false; 
            printf(">>> ");
            int size = (int)getline(&input, &n, stdin);
            input[size - 1] = '\0';
            
            DebuggerCommand command;
            int error = parse_debugger_command(input, &command);

            if (error != 0) {
                // TODO: Print out specific error here.
                printf("Error parsing command!\n");
                continue;
            }

            if (command.type == EXIT) break;
            if (command.type == CONTINUE) {
                pass = true;
                execute_next_instruction(emulator);
                continue;
            }

            execute_debugger_command(&command, emulator, &breakpoints);
            destroy_debugger_command(&command);
        } else {
            execute_next_instruction(emulator);
        }

        render_frame(&(emulator->display));
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
