#ifndef DEBUGGER_H
#define DEBUGGER_H

#define MAX_DEBUGGER_COMMAND_ARGS 3

#include <stdbool.h>
#include "emulator.h"

typedef struct breakpoints_t {
    // TODO: This should be dynamically allocated. Implement a resizable array?
    uint16_t addresses[4096];
    int address_count;
} Breakpoints;

typedef enum debugger_command_type_t {
    CONTINUE,
    EXIT,
    MEMORY,
    NEXT,
    PREVIOUS,
    REGISTER,
    REGISTERS,
    STACK,
    STEP,
    BREAK
} DebuggerCommandType;

typedef struct debugger_command_t {
    DebuggerCommandType type;
    char **args;
    int arg_count;
} DebuggerCommand;

int parse_debugger_command(char *input, DebuggerCommand *command);
int destroy_debugger_command(DebuggerCommand *command);
void execute_debugger_command(DebuggerCommand *command, Emulator *emulator, Breakpoints *breakpoints);
bool should_break(Emulator *emulator, Breakpoints *breakpoints);

#endif