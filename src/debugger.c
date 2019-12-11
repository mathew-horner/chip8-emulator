#include <stdlib.h>
#include <stdint.h>
#include "debugger.h"
#include "memory.h"

void print_register_values(CPU *cpu)
{
    printf("I: 0x%x\n", cpu->I);
    printf("dt: 0x%x\n", cpu->dt);
    printf("st: 0x%x\n", cpu->st);
    for (int i = 0; i < 16; i++)
        printf("V%d: 0x%x\n", i, cpu->registers[i]);
}

void command_memory(Debugger *debugger, DebuggerCommand *command)
{
    if (command->arg_count < 2) {
        printf("Usage: memory <start address> <end address> [chunk size]\n");
        return;
    }

    // FIXME: Need to do better error handling here.
    // Does not currently account for junk values.
    uint16_t start = (uint16_t)strtol(command->args[0], NULL, 0);
    uint16_t end = (uint16_t)strtol(command->args[1], NULL, 0);

    int bytes_per = 1;

    if (command->arg_count >= 3) {
        if (strcmp(command->args[2], "2") == 0) bytes_per = 2;
        else if (strcmp(command->args[2], "4") == 0) bytes_per = 4;
    }

    if (start < 0 || (end + bytes_per) >= MEMORY_SIZE) {
        printf("The memory range specified is outside the bounds of the memory!\n");
        return;
    }

    for (int i = start; i <= end; i += bytes_per) {
        unsigned int memory_value = debugger->emulator->memory.values[i];
        for (int j = 1; j < bytes_per; j++) {
            memory_value <<= 8;
            memory_value |= debugger->emulator->memory.values[i + j];
        }
        printf("[0x%x] 0x%x\n", i, memory_value);
    }
}

void command_next(Debugger *debugger, DebuggerCommand *command)
{
    printf("0x%x\n", next_instruction(debugger->emulator));
}

void command_previous(Debugger *debugger, DebuggerCommand *command)
{
    printf("0x%x\n", previous_instruction(debugger->emulator));
}

void command_register(Debugger *debugger, DebuggerCommand *command)
{
    if (strcmp(command->args[0], "I") == 0)
        printf("I: 0x%x\n", debugger->emulator->cpu.I);
    else if (strcmp(command->args[0], "dt") == 0)
        printf("dt: 0x%x\n", debugger->emulator->cpu.dt);
    else if (strcmp(command->args[0], "st") == 0)
        printf("st: 0x0%x\n", debugger->emulator->cpu.st);
    else {
        // FIXME: Need to do better error handling here.
        // Does not currently account for junk values.
        int v = atoi(command->args[0]);
        if (v < 0 || v > 15) {
            printf("%d is not a valid register number!\n", v);
            return;
        }
        printf("V%d: 0x%x\n", v, debugger->emulator->cpu.registers[v]);
    }
}

void command_registers(Debugger *debugger, DebuggerCommand *command)
{
    print_register_values(&(debugger->emulator->cpu));
}

void command_stack(Debugger *debugger, DebuggerCommand *command)
{
    if (command->arg_count != 1) {
        printf("Usage: stack <command> - Available Commands: full, peek\n");
        return;
    }

    char *stack_command = command->args[0];

    if (strcmp(stack_command, "full") == 0) {
        for (int i = 0; i <= debugger->emulator->cpu.sp; i++)
            printf("[%d] 0x%x\n", i, debugger->emulator->cpu.stack[i]);
    } else if (strcmp(stack_command, "peek") == 0) {
        if (debugger->emulator->cpu.sp >= 0)
            printf("0x%x\n", debugger->emulator->cpu.stack[debugger->emulator->cpu.sp]);
    } else {
        printf("%s is not a valid stack inspection command!", stack_command);
    }
}

void command_step(Debugger *debugger, DebuggerCommand *command)
{
    execute_next_instruction(debugger->emulator);
}

void command_break(Debugger *debugger, DebuggerCommand *command)
{
    if (command->arg_count < 1) {
        printf("Usage: break <command> [command specific options]\n");
        return;
    }
    
    if (strcmp(command->args[0], "list-address") == 0) {
        for (int i = 0; i < debugger->break_address_count; i++)
            if (debugger->break_addresses[i] != 0)
                printf("0x%x\n", debugger->break_addresses[i]);
        return;
    }

    if (!(strcmp(command->args[0], "address") == 0 || strcmp(command->args[0], "remove-address") == 0)) {
        printf("Invalid command!\n");
        return;
    }

    if (command->arg_count != 2) {
        printf("Usage: break %s <memory address (in Hex)>\n", command->args[0]);
        return;
    }

    // FIXME: Need to do better error handling here.
    // Does not currently account for junk values.
    uint16_t address = (uint16_t)strtol(command->args[1], NULL, 0);

    if (strcmp(command->args[0], "address") == 0) {
        debugger->break_addresses[debugger->break_address_count] = address;
        debugger->break_address_count++;
    } else if (strcmp(command->args[0], "remove-address") == 0) {
        int address_index = -1;
        for (int i = 0; i < debugger->break_address_count; i++) {
            if (debugger->break_addresses[i] == address) {
                address_index = i;
                break;
            }
        }

        if (address_index != -1) {
            debugger->break_addresses[address_index] = 0;
        } else {
            printf("Address does not currently have a breakpoint!\n");
        }
    }
}

void (*debugger_command_map[10]) (Debugger *debugger, DebuggerCommand *command) = {
    NULL,
    NULL,
    command_memory,
    command_next,
    command_previous,
    command_register,
    command_registers,
    command_stack,
    command_step,
    command_break
};

// Executes a debugger command against an Emulator instance.
void execute_debugger_command(Debugger *debugger, DebuggerCommand *command)
{
    debugger_command_map[command->type](debugger, command);
}

/*
 * Attempts to parse debugger prompt input into a DebuggerCommand struct.
 * 
 * Returns:
 * 0 - Success.
 * 1 - Invalid command.
 * 2 - Too many arguments (cannot fit inside fixed size command.args buffer).
 */
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
    else if (strcmp(input, "stack") == 0)
        command->type = STACK;
    else if (strcmp(input, "step") == 0)
        command->type = STEP;
    else if (strcmp(input, "break") == 0)
        command->type = BREAK;
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

// Frees all memory allocated by parse_debugger_command.
int destroy_debugger_command(DebuggerCommand *command)
{
    free(command->args);
}

// Returns whether or not the debugger should break with the given state of the emulator.
bool should_break(Debugger *debugger)
{
    for (int i = 0; i < debugger->break_address_count; i++)
        if (debugger->break_addresses[i] == debugger->emulator->cpu.pc)
            return true;
    return false;
}
