#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "emulator.h"
#include "io.h"

#define MAX_DEBUGGER_COMMAND_ARGS 3

typedef enum debugger_command_type_t {
    CONTINUE,
    EXIT,
    MEMORY,
    NEXT,
    PREVIOUS,
    REGISTER,
    REGISTERS,
    STEP,
} DebuggerCommandType;

typedef struct debugger_command_t {
    DebuggerCommandType type;
    char **args;
    int arg_count;
} DebuggerCommand;

int parse_debugger_command(char *input, DebuggerCommand *command)
{
    char *token = strtok(input, " ");

    if (strcmp(input, "continue") == 0)
        command->type = CONTINUE;
    else if (strcmp(input, "exit") == 0)
        command->type = EXIT;
    else if (strcmp(input, "memory") == 0)
        command->type = MEMORY;
    else if (strcmp(input, "next") == 0)
        command->type = NEXT;
    else if (strcmp(input, "previous") == 0)
        command->type = PREVIOUS;
    else if (strcmp(input, "register") == 0)
        command->type = REGISTER;
    else if (strcmp(input, "registers") == 0)
        command->type = REGISTERS;
    else if (strcmp(input, "step") == 0)
        command->type = STEP;
    else
        return 1;
    
    int i = 0;
    command->args = (char **)malloc(MAX_DEBUGGER_COMMAND_ARGS * sizeof(char *));
    token = strtok(NULL, " ");

    while (token != NULL) {
        if (i >= MAX_DEBUGGER_COMMAND_ARGS)
            return 2;
        command->args[i] = token;
        token = strtok(NULL, " ");
        i++;
    }

    command->arg_count = i;
    return 0;
}

int destroy_debugger_command(DebuggerCommand *command)
{
    free(command->args);
}

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
        uint16_t instruction = (uint16_t)strtol(input, NULL, 16);
        execute_instruction(emulator, instruction);
        render_frame(&(emulator->display));
    }
}

void debugger_loop(Emulator *emulator)
{
    char *input;
    size_t n;

    printf("You are running the Chip-8 Emulator in Debug mode.\n");

    while (1) {
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
            printf("Not supported yet.\n");
        } else if (command.type == STEP) {
            execute_next_instruction(emulator);
        } else if (command.type == MEMORY) {
            if (command.arg_count < 2) {
                printf("Usage: memory <start address> <end address> [chunk size]\n");
                continue;
            }

            // FIXME: Need to do better error handling here.
            // Does not currently account for junk values.
            uint16_t start = (uint16_t)strtol(command.args[0], NULL, 0);
            uint16_t end = (uint16_t)strtol(command.args[1], NULL, 0);

            int bytes_per = 1;

            if (command.arg_count >= 3) {
                if (strcmp(command.args[2], "2") == 0) bytes_per = 2;
                else if (strcmp(command.args[2], "4") == 0) bytes_per = 4;
            }

            if (start < 0 || (end + bytes_per) >= MEMORY_SIZE) {
                printf("The memory range specified is outside the bounds of the memory!\n");
                continue;
            }

            for (int i = start; i <= end; i += bytes_per) {
                unsigned int memory_value = emulator->memory.values[i];
                for (int j = 1; j < bytes_per; j++) {
                    memory_value <<= 8;
                    memory_value |= emulator->memory.values[i + j];
                }
                printf("[0x%x] 0x%x\n", i, memory_value);
            }
        } else if (command.type == NEXT) {
            printf("0x%x\n", next_instruction(emulator));
        } else if (command.type == PREVIOUS) {
            printf("0x%x\n", previous_instruction(emulator));
        } else if (command.type == REGISTER) {
            if (strcmp(command.args[0], "I") == 0)
                printf("I: 0x%x\n", emulator->cpu.I);
            else if (strcmp(command.args[0], "dt") == 0)
                printf("dt: 0x%x\n", emulator->cpu.dt);
            else if (strcmp(command.args[0], "st") == 0)
                printf("st: 0x0%x\n", emulator->cpu.st);
            else {
                // FIXME: Need to do better error handling here.
                // Does not currently account for junk values.
                int v = atoi(command.args[0]);
                if (v < 0 || v > 12) {
                    printf("%d is not a valid register number!\n", v);
                    continue;
                }
                printf("V%d: 0x%x\n", v, emulator->cpu.registers[v]);
            }
        } else if (command.type == REGISTERS) {
            print_register_values(&(emulator->cpu));
        }

        destroy_debugger_command(&command);
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
