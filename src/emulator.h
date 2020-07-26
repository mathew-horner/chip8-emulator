#ifndef EMULATOR_H
#define EMULATOR_H

#include "cpu.h"
#include "display.h"
#include "memory.h"

typedef enum instruction_type_t {
    CLS,
    RET,
    JP_ADDR,
    CALL_ADDR,
    SE_VX_BYTE,
    SNE_VX_BYTE,
    SE_VX_VY,
    LD_VX_BYTE,
    ADD_VX_BYTE,
    LD_VX_VY,
    OR_VX_VY,
    AND_VX_VY,
    XOR_VX_VY,
    ADD_VX_VY,
    SUB_VX_VY,
    SHR_VX_VY,
    SUBN_VX_VY,
    SHL_VX_VY,
    SNE_VX_VY,
    LD_I_ADDR,
    JP_V0_ADDR,
    RND_VX_BYTE,
    DRW_VX_VY_NIBBLE,
    SKP_VX,
    SKNP_VX,
    LD_VX_DT,
    LD_VX_K,
    LD_DT_VX,
    LD_ST_VX,
    ADD_I_VX,
    LD_F_VX,
    LD_B_VX,
    LD_I_VX,
    LD_VX_I
} InstructionType;

typedef struct executed_instruction_t ExecutedInstruction;
typedef struct executed_instruction_t {
    InstructionType type;
    void *data;
    ExecutedInstruction *previous;
    uint16_t address;
} ExecutedInstruction;

typedef struct emulator_t {
    CPU cpu;
    Display display;
    uint8_t memory[MEMORY_SIZE];
    
    bool key_state[16];
    bool waiting_for_key;
    int key_register;

    bool record_execution;
    ExecutedInstruction *execution_record_ptr;
} Emulator;

void initialize_emulator(Emulator *emulator);
void initialize_emulator_no_display(Emulator *emulator);
void execute_instruction(Emulator *emulator, uint16_t instruction);
void execute_next_instruction(Emulator *emulator);
uint16_t next_instruction(Emulator *emulator);
uint16_t previous_instruction(Emulator *emulator);
int revert_last_instruction(Emulator *emulator);

// TODO: These probably don't need to be forward declared here.

void do_CLS(Emulator *emulator, uint16_t instruction);
void do_RET(Emulator *emulator, uint16_t instruction);
void do_JP_ADDR(Emulator *emulator, uint16_t instruction);
void do_CALL_ADDR(Emulator *emulator, uint16_t instruction);
void do_SE_VX_BYTE(Emulator *emulator, uint16_t instruction);
void do_SNE_VX_BYTE(Emulator *emulator, uint16_t instruction);
void do_SE_VX_VY(Emulator *emulator, uint16_t instruction);
void do_LD_VX_BYTE(Emulator *emulator, uint16_t instruction);
void do_ADD_VX_BYTE(Emulator *emulator, uint16_t instruction);
void do_LD_VX_VY(Emulator *emulator, uint16_t instruction);
void do_OR_VX_VY(Emulator *emulator, uint16_t instruction);
void do_AND_VX_VY(Emulator *emulator, uint16_t instruction);
void do_XOR_VX_VY(Emulator *emulator, uint16_t instruction);
void do_ADD_VX_VY(Emulator *emulator, uint16_t instruction);
void do_SUB_VX_VY(Emulator *emulator, uint16_t instruction);
void do_SHR_VX_VY(Emulator *emulator, uint16_t instruction);
void do_SUBN_VX_VY(Emulator *emulator, uint16_t instruction);
void do_SHL_VX_VY(Emulator *emulator, uint16_t instruction);
void do_SNE_VX_VY(Emulator *emulator, uint16_t instruction);
void do_LD_I_ADDR(Emulator *emulator, uint16_t instruction);
void do_JP_V0_ADDR(Emulator *emulator, uint16_t instruction);
void do_RND_VX_BYTE(Emulator *emulator, uint16_t instruction);
void do_DRW_VX_VY_NIBBLE(Emulator *emulator, uint16_t instruction);
void do_SKP_VX(Emulator *emulator, uint16_t instruction);
void do_SKNP_VX(Emulator *emulator, uint16_t instruction);
void do_LD_VX_DT(Emulator *emulator, uint16_t instruction);
void do_LD_VX_K(Emulator *emulator, uint16_t instruction);
void do_LD_DT_VX(Emulator *emulator, uint16_t instruction);
void do_LD_ST_VX(Emulator *emulator, uint16_t instruction);
void do_ADD_I_VX(Emulator *emulator, uint16_t instruction);
void do_LD_F_VX(Emulator *emulator, uint16_t instruction);
void do_LD_B_VX(Emulator *emulator, uint16_t instruction);
void do_LD_I_VX(Emulator *emulator, uint16_t instruction);
void do_LD_VX_I(Emulator *emulator, uint16_t instruction);

void undo_CLS(Emulator *emulator, void *data);
void undo_RET(Emulator *emulator, void *data);
void undo_JP_ADDR(Emulator *emulator, void *data);
void undo_CALL_ADDR(Emulator *emulator, void *data);
void undo_SE_VX_BYTE(Emulator *emulator, void *data);
void undo_SNE_VX_BYTE(Emulator *emulator, void *data);
void undo_SE_VX_VY(Emulator *emulator, void *data);
void undo_LD_VX_BYTE(Emulator *emulator, void *data);
void undo_ADD_VX_BYTE(Emulator *emulator, void *data);
void undo_LD_VX_VY(Emulator *emulator, void *data);
void undo_OR_VX_VY(Emulator *emulator, void *data);
void undo_AND_VX_VY(Emulator *emulator, void *data);
void undo_XOR_VX_VY(Emulator *emulator, void *data);
void undo_ADD_VX_VY(Emulator *emulator, void *data);
void undo_SUB_VX_VY(Emulator *emulator, void *data);
void undo_SHR_VX_VY(Emulator *emulator, void *data);
void undo_SUBN_VX_VY(Emulator *emulator, void *data);
void undo_SHL_VX_VY(Emulator *emulator, void *data);
void undo_SNE_VX_VY(Emulator *emulator, void *data);
void undo_LD_I_ADDR(Emulator *emulator, void *data);
void undo_JP_V0_ADDR(Emulator *emulator, void *data);
void undo_RND_VX_BYTE(Emulator *emulator, void *data);
void undo_DRW_VX_VY_NIBBLE(Emulator *emulator, void *data);
void undo_SKP_VX(Emulator *emulator, void *data);
void undo_SKNP_VX(Emulator *emulator, void *data);
void undo_LD_VX_DT(Emulator *emulator, void *data);
void undo_LD_VX_K(Emulator *emulator, void *data);
void undo_LD_DT_VX(Emulator *emulator, void *data);
void undo_LD_ST_VX(Emulator *emulator, void *data);
void undo_ADD_I_VX(Emulator *emulator, void *data);
void undo_LD_F_VX(Emulator *emulator, void *data);
void undo_LD_B_VX(Emulator *emulator, void *data);
void undo_LD_I_VX(Emulator *emulator, void *data);
void undo_LD_VX_I(Emulator *emulator, void *data);

void (*undo_functions[34]) (Emulator *emulator, void *data);

#endif
