#include <stdlib.h>
#include <stdint.h>
#include "debugger.h"
#include "disassemble.h"
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
        unsigned int memory_value = debugger->emulator->memory[i];
        for (int j = 1; j < bytes_per; j++) {
            memory_value <<= 8;
            memory_value |= debugger->emulator->memory[i + j];
        }
        printf("[0x%x] 0x%x\n", i, memory_value);
    }
}

void print_instruction(uint16_t instruction, uint16_t address, bool arrow)
{
    char *assembly = disassemble_instruction(instruction);
    printf("[0x%x] %s%s\n", address, arrow ? "> " : "", assembly);
    free(assembly);
}

void command_next(Debugger *debugger, DebuggerCommand *command)
{
    print_instruction(next_instruction(debugger->emulator), debugger->emulator->cpu.pc, false);
}

void command_previous(Debugger *debugger, DebuggerCommand *command)
{
    if (debugger->emulator->cpu.previous_pc == 0) {
        printf("No instructions have been executed yet!\n");
        return;
    }
    print_instruction(previous_instruction(debugger->emulator), debugger->emulator->cpu.previous_pc, false);
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

void command_help(Debugger *debugger, DebuggerCommand *command)
{
    printf("exit: Exits the process.\n");
    printf("step: Executes the next instruction.\n");
    printf("next: Displays the next instruction.\n");
    printf("previous: Displays the previous instruction.\n");
    printf("register <v>: Displays the contents of a single CPU register (can be dt, st, I, or any number 0-15).\n");
    printf("registers: Displays the contents of every CPU register.\n");
    printf("memory <start> <end> [chunk size]: Displays the contents of memory from addresses start:end (given in hexidecimal form). You can also supply a chunk size argument of 1, 2, or 4 - which will specify whether the command will display the memory in 8-bit, 16-bit, or 32-bit format.\n");
    printf("    Example: memory 0x200 0x203 2\n\n");
    printf("    Output\n");
    printf("    [0x200] 0x2f8b\n");
    printf("    [0x202] 0xffff\n");
    printf("break address <memory address (in Hex)>: Sets a breakpoint for the instruction at the given memory address.\n");
    printf("break remove-address <memory address (in Hex)>: Removes the breakpoint at the given memory  address.\n");
    printf("break list-address: Lists all addresses that have breakpoints.\n");
    printf("continue: Resumes execution until the next breakpoint is hit.\n");
    printf("stack full: Displays the entire stack.\n");
    printf("stack peek: Displays the top value of the stack.\n");
    printf("context <n>: Shows the n instructions surrounding the instruction being pointed to by the PC.\n");
}

void command_context(Debugger *debugger, DebuggerCommand *command)
{
    int n = atoi(command->args[0]);
    int pc = debugger->emulator->cpu.pc;
    int start = pc - (n * 2);
    int end = pc + (n * 2) + 1;

    if (start < PROGRAM_OFFSET) start = PROGRAM_OFFSET;

    // todo: this should only read until the end of program ROM.
    if (end >= MEMORY_SIZE - 1) end = MEMORY_SIZE - 2;

    for (int i = start; i < end; i += 2) {
        uint16_t instruction = debugger->emulator->memory[i] << 8 | debugger->emulator->memory[i + 1];
        print_instruction(instruction, i, i == pc);
    }
}

void (*debugger_command_map[12]) (Debugger *debugger, DebuggerCommand *command) = {
    NULL,
    NULL,
    command_memory,
    command_next,
    command_previous,
    command_register,
    command_registers,
    command_stack,
    command_step,
    command_break,
    command_help,
    command_context
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
    else if (strcmp(input, "help") == 0)
        command->type = HELP;
    else if (strcmp(input, "context") == 0)
        command->type = CONTEXT;
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
    // TODO: Use a set implementation here for O(1) lookup?
    for (int i = 0; i < debugger->break_address_count; i++)
        if (debugger->break_addresses[i] == debugger->emulator->cpu.pc)
            return true;
    return false;
}
