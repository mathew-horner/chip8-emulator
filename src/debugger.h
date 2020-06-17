#ifndef DEBUGGER_H
#define DEBUGGER_H

#define MAX_DEBUGGER_COMMAND_ARGS 3

#include <stdbool.h>
#include "emulator.h"
#include "set.h"

typedef struct debugger_t {
    Emulator *emulator;
    Set *break_addresses;
} Debugger;

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
    BREAK,
    HELP
} DebuggerCommandType;

typedef struct debugger_command_t {
    DebuggerCommandType type;
    char **args;
    int arg_count;
} DebuggerCommand;

void initialize_debugger(Debugger *debugger);
int parse_debugger_command(char *input, DebuggerCommand *command);
int destroy_debugger_command(DebuggerCommand *command);
void execute_debugger_command(Debugger *debugger, DebuggerCommand *command);
bool should_break(Debugger *debugger);

#endif