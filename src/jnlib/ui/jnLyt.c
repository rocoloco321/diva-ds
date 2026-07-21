#include "common.h"
#include "oam.h"
#include "heap.h"
#include "jnUIUtil.h"
#include "jnLyt.h"
#include "print.h"

#include <nitro/code16.h>

static void initLayoutElements(jnui_layout_element_t* elements, int count)
{
    for (int i = 0; i < count; i++)
    {
        elements->visible = TRUE;
        elements->offsetX = 0;
        elements->offsetY = 0;
        elements->usePosition = FALSE;
        elements->positionX = 0;
        elements->positionY = 0;
        elements->useMtx = FALSE;
        MTX_Identity22(&elements->baseMtx);
        MTX_Identity22(&elements->affineMtx);
        elements->useDoubleAffine = TRUE;
        elements->subElement = -1;
        elements->label = 0;
        elements++;
    }
}

void jnui_initLayout(NNSFndHeapHandle heapHandle, const jnui_bncl_res_t* bncl, const jnui_bnll_res_t* bnll,
                     jnui_layout_element_t** layoutElements)
{
    u16 nrElements = bncl->header.nrElements;
    if (bnll)
        nrElements += bnll->header.nrElements;
    *layoutElements = (jnui_layout_element_t*)heap_alloc(heapHandle, nrElements * sizeof(jnui_layout_element_t));
    initLayoutElements(*layoutElements, nrElements);
}

static u16 renderElementIntern(int elementIdx, const NNSG2dCellDataBank* cellDataBank, const jnui_bncl_res_t* bncl,
                               const jnui_bnll_res_t* bnll, oam_buf_t* oamBuffer, jnui_layout_element_t* layoutElement,
                               BOOL increaseAffine)
{
    int width;
    int height;
    jnui_coord_t x;
    jnui_coord_t y;
    MtxFx22* baseMtx;
    u16 affineIdx;
    const NNSG2dCellDataWithBR* cellData;
    const jnui_bnll_res_element_t* bnllElement;
    const jnui_bncl_res_element_t* element;
    u16 objCount;
    NNSG2dFVec2 pos;
    if (layoutElement->subElement != -1)
        renderElementIntern(layoutElement->subElement + bncl->header.nrElements, cellDataBank, bncl, bnll, oamBuffer,
                            layoutElement - elementIdx + bncl->header.nrElements + layoutElement->subElement,
                            increaseAffine);
    baseMtx = NULL;
    if (!layoutElement->visible)
        return 0;
    if (elementIdx >= bncl->header.nrElements)
    {
        bnllElement = jnui_getBnllElement(bnll, elementIdx - bncl->header.nrElements);
        cellData = layoutElement->label->cellData;
        x = bnllElement->x;
        y = bnllElement->y;
        width = cellData->boundingRect.maxX - cellData->boundingRect.minX;
        height = cellData->boundingRect.maxY - cellData->boundingRect.minY;
        pos.x = x.coord * FX32_ONE;
        pos.y = y.coord * FX32_ONE;
    }
    else
    {
        element = jnui_getBnclElement(bncl, elementIdx);
        cellData = (const NNSG2dCellDataWithBR*)NNS_G2dGetCellDataByIdx(cellDataBank, element->cellId);
        x = element->x;
        y = element->y;
        width = cellData->boundingRect.maxX - cellData->boundingRect.minX;
        height = cellData->boundingRect.maxY - cellData->boundingRect.minY;
        pos.x = (jnui_calcOriginCoordX(element->x, width) + (width >> 1)) * FX32_ONE;
        pos.y = (jnui_calcOriginCoordY(element->y, height) + (height >> 1)) * FX32_ONE;
    }
    if (layoutElement->usePosition)
    {
        if (elementIdx >= bncl->header.nrElements)
        {
            pos.x = layoutElement->positionX * FX32_ONE;
            pos.y = layoutElement->positionY * FX32_ONE;
        }
        else
        {
            pos.x = (layoutElement->positionX + (width >> 1) * (1 - x.origin)) * FX32_ONE;
            pos.y = (layoutElement->positionY + (height >> 1) * (1 - y.origin)) * FX32_ONE;
        }
    }
    pos.x += layoutElement->offsetX * FX32_ONE;
    pos.y += layoutElement->offsetY * FX32_ONE;
    if ((pos.x >> FX32_SHIFT) + width < 0 || (pos.x >> FX32_SHIFT) - width > HW_LCD_WIDTH ||
        (pos.y >> FX32_SHIFT) + height < 0 || (pos.y >> FX32_SHIFT) - height > HW_LCD_HEIGHT)
    {
        return 0;
    }
    affineIdx = oam_getAffineCount(oamBuffer);
    if (layoutElement->useMtx)
    {
        baseMtx = &layoutElement->affineMtx;
        G2_SetOBJAffine(&((GXOamAffine*)oamBuffer->oam)[affineIdx], &layoutElement->baseMtx);
        if (increaseAffine)
            oam_increaseAffineCount(oamBuffer);
    }
    objCount = NNS_G2dMakeCellToOams(&oamBuffer->oam[oamBuffer->objCount], 128 - oamBuffer->objCount,
                                     (const NNSG2dCellData*)cellData, baseMtx, &pos, affineIdx,
                                     layoutElement->useDoubleAffine);
    oamBuffer->objCount += objCount;
    return objCount;
}

void jnui_renderElements(const jnui_bncl_res_t* bncl, const NNSG2dCellDataBank* cellDataBank,
                         const jnui_bnll_res_t* bnll, oam_buf_t* oamBuffer, jnui_layout_element_t* layoutElements)
{
    for (int i = bncl->header.nrElements - 1; i >= 0; i--)
        renderElementIntern(i, cellDataBank, bncl, bnll, oamBuffer, &layoutElements[i], TRUE);
}

void jnui_renderElementsNoIncrAffine(const jnui_bncl_res_t* bncl, const NNSG2dCellDataBank* cellDataBank,
                                     const jnui_bnll_res_t* bnll, oam_buf_t* oamBuffer,
                                     jnui_layout_element_t* layoutElements)
{
    for (int i = bncl->header.nrElements - 1; i >= 0; i--)
        renderElementIntern(i, cellDataBank, bncl, bnll, oamBuffer, &layoutElements[i], FALSE);
}

void jnui_renderElement(int elementIdx, const jnui_bncl_res_t* bncl, const NNSG2dCellDataBank* cellDataBank,
                        const jnui_bnll_res_t* bnll, oam_buf_t* oamBuffer, jnui_layout_element_t* layoutElements)
{
    renderElementIntern(elementIdx, cellDataBank, bncl, bnll, oamBuffer, &layoutElements[elementIdx], TRUE);
}

void jnui_setElementAffine(jnui_layout_element_t* layoutElement, int rotation, fx32 scale)
{
    int offset;
    fx32 invScale;
    MtxFx22* baseMtx;
    MtxFx22* affineMtx;

    offset = 2 * (rotation >> 4);
    baseMtx = &layoutElement->baseMtx;
    affineMtx = &layoutElement->affineMtx;

    invScale = FX_Div(FX32_ONE, scale);
    baseMtx->_00 = FX_TRUNC_MUL(FX_SinCosTable_[offset + 1], invScale);
    baseMtx->_01 = FX_TRUNC_MUL(FX_SinCosTable_[offset], invScale);
    baseMtx->_10 = -baseMtx->_01;
    baseMtx->_11 = baseMtx->_00;
    MTX_Rot22(affineMtx, FX_SinIdx(rotation), FX_CosIdx(rotation));
    affineMtx->_00 = FX_Mul(affineMtx->_00, scale);
    affineMtx->_01 = FX_Mul(affineMtx->_01, scale);
    affineMtx->_10 = FX_Mul(affineMtx->_10, scale);
    affineMtx->_11 = FX_Mul(affineMtx->_11, scale);
}

BOOL jnui_getCellIsTranslucent(int elementIdx, const NNSG2dCellDataBank* cellDataBank, const jnui_bncl_res_t* bncl)
{
    const NNSG2dCellData* cellData = NNS_G2dGetCellDataByIdx(cellDataBank,
                                                             jnui_getBnclElement2(bncl, elementIdx)->cellId);
    return ((cellData->pOamAttrArray->attr0 & GX_OAM_ATTR01_MODE_MASK) >> GX_OAM_ATTR01_MODE_SHIFT) == GX_OAM_MODE_XLU;
}

void jnui_setCellIsTranslucent(int elementIdx, BOOL translucent, const NNSG2dCellDataBank* cellDataBank,
                               const jnui_bncl_res_t* bncl)
{
    int i;
    const NNSG2dCellData* cellData = NNS_G2dGetCellDataByIdx(cellDataBank,
                                                             jnui_getBnclElement2(bncl, elementIdx)->cellId);
    NNSG2dCellOAMAttrData* pOamData = cellData->pOamAttrArray;
    for (i = 0; i < cellData->numOAMAttrs; i++)
    {
        pOamData->attr0 &= ~GX_OAM_ATTR01_MODE_MASK;
        if (translucent)
            pOamData->attr0 |= GX_OAM_MODE_XLU << GX_OAM_ATTR01_MODE_SHIFT;
        pOamData++;
    }
}

u8 jnui_getCellColorParam(int elementIdx, const NNSG2dCellDataBank* cellDataBank, const jnui_bncl_res_t* bncl)
{
    const NNSG2dCellData* cellData = NNS_G2dGetCellDataByIdx(cellDataBank,
                                                             jnui_getBnclElement2(bncl, elementIdx)->cellId);
    return (cellData->pOamAttrArray->attr2 & GX_OAM_ATTR2_CPARAM_MASK) >> GX_OAM_ATTR2_CPARAM_SHIFT;
}

void jnui_setCellColorParam(int elementIdx, int colorParam, const NNSG2dCellDataBank* cellDataBank, const jnui_bncl_res_t* bncl)
{
    int i;
    const NNSG2dCellData* cellData;
    NNSG2dCellOAMAttrData* pOamData;
    cellData = NNS_G2dGetCellDataByIdx(cellDataBank, jnui_getBnclElement2(bncl, elementIdx)->cellId);
    pOamData = cellData->pOamAttrArray;

    for (i = 0; i < cellData->numOAMAttrs; i++)
    {
        pOamData->attr2 &= ~GX_OAM_ATTR2_CPARAM_MASK;
        pOamData->attr2 |= colorParam << GX_OAM_ATTR2_CPARAM_SHIFT;
        pOamData++;
    }
}

u8 jnui_getCellPriority(int elementIdx, const NNSG2dCellDataBank* cellDataBank, const jnui_bncl_res_t* bncl)
{
    const NNSG2dCellData* cellData
        = NNS_G2dGetCellDataByIdx(cellDataBank, jnui_getBnclElement2(bncl, elementIdx)->cellId);
    return (cellData->pOamAttrArray->attr2 & GX_OAM_ATTR2_PRIORITY_MASK) >> GX_OAM_ATTR2_PRIORITY_SHIFT;
}

void jnui_setCellPriority(int elementIdx, u8 priority, const NNSG2dCellDataBank* cellDataBank,
                          const jnui_bncl_res_t* bncl)
{
    int i;
    const NNSG2dCellData* cellData;
    NNSG2dCellOAMAttrData* pOamData;

    cellData = NNS_G2dGetCellDataByIdx(cellDataBank, jnui_getBnclElement2(bncl, elementIdx)->cellId);
    pOamData = cellData->pOamAttrArray;
    for (i = 0; i < cellData->numOAMAttrs; i++)
    {
        pOamData->attr2 &= ~GX_OAM_ATTR2_PRIORITY_MASK;
        pOamData->attr2 |= priority << GX_OAM_ATTR2_PRIORITY_SHIFT;
        pOamData++;
    }
}

u16 jnui_getCellCharName(int elementIdx, const NNSG2dCellDataBank* cellDataBank, const jnui_bncl_res_t* bncl)
{
    const NNSG2dCellData* cellData
        = NNS_G2dGetCellDataByIdx(cellDataBank, jnui_getBnclElement2(bncl, elementIdx)->cellId);
    return (cellData->pOamAttrArray->attr2 & GX_OAM_ATTR2_NAME_MASK) >> GX_OAM_ATTR2_NAME_SHIFT;
}

static const u8 sObjCharCount[12] =
{
    1, 4, 16, 64,
    2, 4, 8, 32,
    2, 4, 8, 32
};

void sub_212481C(int srcElementIdx, const NNSG2dCellDataBank* srcCellDataBank, const NNSG2dCharacterData* srcCharData,
                 const NNSG2dPaletteData* srcPlttData, const jnui_bncl_res_t* srcBncl, int engine, int dstElementId,
                 const NNSG2dCellDataBank* dstCellDataBank, const jnui_bncl_res_t* dstBncl)
{
    int i; // [sp+18h] [bp-28h]
    int count;
    u32 v16; // r7
    const NNSG2dCellData* v9; // r6
    u32 size;
    u32 v15; // r5
    int v23; // r3
    void* v22; // r2
    int v17; // r1
    u32 v21; // r1
    int v18; // r0
    u32 shape;
    u32 v27; // [sp+Ch] [bp-34h]
    void* v28; // [sp+10h] [bp-30h]
    const NNSG2dCellData* v29; // [sp+14h] [bp-2Ch]
    u8 v31[16]; // [sp+1Ch] [bp-24h] BYREF
    u32 tmp;

    v29 = NNS_G2dGetCellDataByIdx(srcCellDataBank, jnui_getBnclElement2(srcBncl, srcElementIdx)->cellId);
    v9 = NNS_G2dGetCellDataByIdx(dstCellDataBank, jnui_getBnclElement2(dstBncl, dstElementId)->cellId);
    count = v29->numOAMAttrs;
    for (i = 0; i < 16; i++)
        v31[i] = 0;
    for (i = 0; i < count; i++)
    {
        tmp = v29->pOamAttrArray[i].attr2;
        v27 = (v9->pOamAttrArray[i].attr2 & GX_OAM_ATTR2_NAME_MASK) * 32;
        shape = (u32)(v29->pOamAttrArray[i].attr0 & GX_OAM_ATTR01_SHAPE_MASK) >> GX_OAM_ATTR01_SHAPE_SHIFT;
        size = ((v29->pOamAttrArray[i].attr1 << 16) & (vu32)GX_OAM_ATTR01_SIZE_MASK) >> GX_OAM_ATTR01_SIZE_SHIFT;
        v15 = sObjCharCount[shape * 4 + size] * 32;
        v28 = (u8*)srcCharData->pRawData + ((tmp & GX_OAM_ATTR2_NAME_MASK) << 5);
        DC_FlushRange(v28, v15);
        if (!engine)
            NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_2D_OBJ_CHAR_MAIN, v27, v28, v15);
        else
            NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_2D_OBJ_CHAR_SUB, v27, v28, v15);
        v16 = (u32)(tmp & GX_OAM_ATTR2_CPARAM_MASK) >> GX_OAM_ATTR2_CPARAM_SHIFT;
        v31[v16] = 1;
        v9->pOamAttrArray[i].attr2 &= ~GX_OAM_ATTR2_CPARAM_MASK;
        v9->pOamAttrArray[i].attr2 |= v16 << GX_OAM_ATTR2_CPARAM_SHIFT;
    }
    v17 = 0;
    v18 = 0;
    for (i = 0; i < 16; i++)
    {
        if (v31[i] && v17 + v18 < 15)
            ++v18;
        else if (v18 > 0)
        {
            v21 = 32 * v17;
            v22 = (u8*)srcPlttData->pRawData + v21;
            v23 = 32 * v18;
            if (!engine)
                NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_2D_OBJ_PLTT_MAIN, v21, v22, v23);
            else
                NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_2D_OBJ_PLTT_SUB, v21, v22, v23);
            v17 = i + 1;
            v18 = 0;
        }
        else
            v17 = i + 1;
    }
}

void jnui_fillPlaceholder(int srcElementIdx, const NNSG2dCellDataBank* srcCellDataBank,
                          const NNSG2dCharacterData* srcCharData, const jnui_bncl_res_t* srcBncl,
                          int engine, int dstElementId, const NNSG2dCellDataBank* dstCellDataBank,
                          const jnui_bncl_res_t* dstBncl)
{
    int v30; // [sp+18h] [bp-28h]
    u16 count;
    u32 size;
    u32 v15; // r5
    u32 shape;
    u32 v27; // [sp+Ch] [bp-34h]
    void* v28; // [sp+10h] [bp-30h]
    const NNSG2dCellData* v29; // [sp+14h] [bp-2Ch]
    const NNSG2dCellData* v9; // r6

    v29 = NNS_G2dGetCellDataByIdx(srcCellDataBank, jnui_getBnclElement2(srcBncl, srcElementIdx)->cellId);
    v9 = NNS_G2dGetCellDataByIdx(dstCellDataBank, jnui_getBnclElement2(dstBncl, dstElementId)->cellId);
    count = v29->numOAMAttrs;
    for (v30 = 0; v30 < count; v30++)
    {
        v27 = (v9->pOamAttrArray[v30].attr2 & GX_OAM_ATTR2_NAME_MASK) * 32;
        shape = (u32)(v29->pOamAttrArray[v30].attr0 & GX_OAM_ATTR01_SHAPE_MASK) >> GX_OAM_ATTR01_SHAPE_SHIFT;
        size = ((v29->pOamAttrArray[v30].attr1 << 16) & (vu32)GX_OAM_ATTR01_SIZE_MASK) >> GX_OAM_ATTR01_SIZE_SHIFT;
        v15 = sObjCharCount[shape * 4 + size] * 32;
        v28 = (u8*)srcCharData->pRawData + ((v29->pOamAttrArray[v30].attr2 & GX_OAM_ATTR2_NAME_MASK) << 5);
        DC_FlushRange(v28, v15);
        if (!engine)
            NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_2D_OBJ_CHAR_MAIN, v27, v28, v15);
        else
            NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_2D_OBJ_CHAR_SUB, v27, v28, v15);
    }
}

void sub_212463C(int srcElementIdx, const NNSG2dCellDataBank* srcCellDataBank, const NNSG2dCharacterData* srcCharData,
                 const NNSG2dPaletteData* srcPlttData, const jnui_bncl_res_t* srcBncl, int engine, int dstElementId,
                 const NNSG2dCellDataBank* dstCellDataBank, const jnui_bncl_res_t* dstBncl)
{
    int i;
    u16 count;
    u32 size;
    u32 charDataLength;
    void* palData;
    u32 dstPlttAddr;
    u32 shape;
    u32 charDataVramDst;
    void* charDataSrc;
    const NNSG2dCellData* srcCellData;
    const NNSG2dCellData* dstCellData;

    srcCellData = NNS_G2dGetCellDataByIdx(srcCellDataBank, jnui_getBnclElement2(srcBncl, srcElementIdx)->cellId);
    dstCellData = NNS_G2dGetCellDataByIdx(dstCellDataBank, jnui_getBnclElement2(dstBncl, dstElementId)->cellId);
    count = srcCellData->numOAMAttrs;
    for (i = 0; i < count; i++)
    {
        charDataVramDst = (dstCellData->pOamAttrArray[i].attr2 & GX_OAM_ATTR2_NAME_MASK) * 32;
        shape = (u32)(srcCellData->pOamAttrArray[i].attr0 & GX_OAM_ATTR01_SHAPE_MASK) >> GX_OAM_ATTR01_SHAPE_SHIFT;
        size = ((srcCellData->pOamAttrArray[i].attr1 << 16) & (vu32)GX_OAM_ATTR01_SIZE_MASK) >>
            GX_OAM_ATTR01_SIZE_SHIFT;
        charDataLength = sObjCharCount[shape * 4 + size] * 32;
        charDataSrc = (u8*)srcCharData->pRawData
            + ((srcCellData->pOamAttrArray[i].attr2 & GX_OAM_ATTR2_NAME_MASK) * 32);
        DC_FlushRange(charDataSrc, charDataLength);
        if (engine == 0)
            NNS_GfdRegisterNewVramTransferTask(
                NNS_GFD_DST_2D_OBJ_CHAR_MAIN, charDataVramDst, charDataSrc, charDataLength);
        else
            NNS_GfdRegisterNewVramTransferTask(
                NNS_GFD_DST_2D_OBJ_CHAR_SUB, charDataVramDst, charDataSrc, charDataLength);
    }
    dstPlttAddr =
        ((u32)(dstCellData->pOamAttrArray[0].attr2 & GX_OAM_ATTR2_CPARAM_MASK) >> GX_OAM_ATTR2_CPARAM_SHIFT) * 32;
    palData = (u8*)srcPlttData->pRawData
        + ((u32)(srcCellData->pOamAttrArray[0].attr2 & GX_OAM_ATTR2_CPARAM_MASK) >> GX_OAM_ATTR2_CPARAM_SHIFT) * 32;
    NNS_GfdRegisterNewVramTransferTask(engine == 0 ? NNS_GFD_DST_2D_OBJ_PLTT_MAIN : NNS_GFD_DST_2D_OBJ_PLTT_SUB,
                                       dstPlttAddr, palData, 32);
}

#include <nitro/codereset.h>
