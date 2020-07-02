#ifndef TIME_TRAVEL_H
#define TIME_TRAVEL_H

#include <stdlib.h>
#include "emulator.h"

typedef enum t_instruction_type {
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

typedef struct t_executed_instruction {
    InstructionType type;
    void *data;
    ExecutedInstruction *previous;
} ExecutedInstruction;


void (*undo_functions[33]) (Emulator *emulator, void *data);

#endif
