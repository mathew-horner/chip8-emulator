#ifndef DEBUGGER_H
#define DEBUGGER_H

#define MAX_DEBUGGER_COMMAND_ARGS 3

#include "emulator.h"

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

int parse_debugger_command(char *input, DebuggerCommand *command);
int destroy_debugger_command(DebuggerCommand *command);
void execute_debugger_command(DebuggerCommand *command, Emulator *emulator);

#endif