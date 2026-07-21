#pragma once
#include "jnLyt.h"

int jnui_calcOriginCoordX(jnui_coord_t coord, int width);
int jnui_calcOriginCoordY(jnui_coord_t coord, int height);

int jnui_checkTouch(const jnui_bnbl_res_t* bnbl, int x, int y);

jnui_bnll_res_element_t* jnui_getBnllElement(const jnui_bnll_res_t* bnll, int idx);
jnui_bncl_res_element_t* jnui_getBnclElement(const jnui_bncl_res_t* bncl, int idx);
jnui_bnbl_res_element_t* jnui_getBnblElement(jnui_bnbl_res_t* bnbl, int elementId);

int jnui_getBnblElementCount(const jnui_bnbl_res_t* bnbl);

//and this particular function should belong in some other file???
// static inline int sub_20291C4(u32 x, u32 y, const jn_ui_bnbl* bnbl)
// {
// 	return jn_ui_checkTouch(bnbl, x, y);
// }

static inline void jnui_registerObjCharVramTransfer(int engine, int vramOffset, void* src, int length)
{
    DC_FlushRange(src, length);

    if (engine == 0)
        NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_2D_OBJ_CHAR_MAIN, vramOffset, src, length);
    else
        NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_2D_OBJ_CHAR_SUB, vramOffset, src, length);
}

static inline const jnui_bncl_res_element_t* jn_ui_getBnclElement3(int elementId, const jnui_bncl_res_t* bncl)
{
  return jnui_getBnclElement(bncl, elementId);
}

inline int jn_ui_checkTouch2(u32 x, u32 y, const jnui_bnbl_res_t* bnbl)
{
  return jnui_checkTouch(bnbl, x, y);
}
