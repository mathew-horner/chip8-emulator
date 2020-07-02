#include "time_travel.h"

void undo_CLS(Emulator *emulator, void *data)
{

}

void undo_RET(Emulator *emulator, void *data)
{

}

void undo_JP_ADDR(Emulator *emulator, void *data)
{
    emulator->cpu.pc = *((int*)data);
    free(data);
}

void undo_CALL_ADDR(Emulator *emulator, void *data)
{

}

void undo_SE_VX_BYTE(Emulator *emulator, void *data)
{

}

void undo_SNE_VX_BYTE(Emulator *emulator, void *data)
{

}

void undo_SE_VX_VY(Emulator *emulator, void *data)
{

}

void undo_LD_VX_BYTE(Emulator *emulator, void *data)
{

}

void undo_ADD_VX_BYTE(Emulator *emulator, void *data)
{

}

void undo_LD_VX_VY(Emulator *emulator, void *data)
{

}

void undo_OR_VX_VY(Emulator *emulator, void *data)
{

}

void undo_AND_VX_VY(Emulator *emulator, void *data)
{

}

void undo_XOR_VX_VY(Emulator *emulator, void *data)
{

}

void undo_ADD_VX_VY(Emulator *emulator, void *data)
{

}

void undo_SUB_VX_VY(Emulator *emulator, void *data)
{

}

void undo_SHR_VX_VY(Emulator *emulator, void *data)
{

}

void undo_SUBN_VX_VY(Emulator *emulator, void *data)
{

}

void undo_SHL_VX_VY(Emulator *emulator, void *data)
{

}

void undo_SNE_VX_VY(Emulator *emulator, void *data)
{

}

void undo_LD_I_ADDR(Emulator *emulator, void *data)
{

}

void undo_JP_V0_ADDR(Emulator *emulator, void *data)
{

}

void undo_RND_VX_BYTE(Emulator *emulator, void *data)
{

}

void undo_DRW_VX_VY_NIBBLE(Emulator *emulator, void *data)
{

}

void undo_SKP_VX(Emulator *emulator, void *data)
{

}

void undo_SKNP_VX(Emulator *emulator, void *data)
{

}

void undo_LD_VX_DT(Emulator *emulator, void *data)
{

}

void undo_LD_VX_K(Emulator *emulator, void *data)
{

}

void undo_LD_DT_VX(Emulator *emulator, void *data)
{

}

void undo_LD_ST_VX(Emulator *emulator, void *data)
{

}

void undo_ADD_I_VX(Emulator *emulator, void *data)
{

}

void undo_LD_F_VX(Emulator *emulator, void *data)
{

}

void undo_LD_B_VX(Emulator *emulator, void *data)
{

}

void undo_LD_I_VX(Emulator *emulator, void *data)
{

}

void undo_LD_VX_I(Emulator *emulator, void *data)
{

}
