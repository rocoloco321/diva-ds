#include "common.h"
#include "heap.h"
#include "main2d.h"
#include "jnUIUtil.h"
#include "jnTextRender.h"
#include "jnLabel.h"

#pragma auto_inline off

#include <nitro/code16.h>

u32 jnui_initLabel(NNSFndHeapHandle heapHandle, int elementIdx, const jnui_bnll_res_t* bnll, int width, int height,
                   int parent, const NNSG2dCellDataBank* cellDataBank, const jnui_bncl_res_t* bncl,
                   jnui_layout_element_t* layoutElements, u32 vramLeft)
{
    NNSG2dCellDataWithBR* cellData;
    jnui_label_t* rtData;
    int i;
    int objCount;
    int x;
    int y;
    int priority;
    BOOL translucent;
    int tileW;
    int tileH;
    const jnui_bnll_res_element_t* element;
    u16 bnclElementCount;
    GXOamAttr tmp[128];
    bnclElementCount = jn_ui_getBnclElementCount(bncl);
    layoutElements[parent].subElement = elementIdx;
    rtData = heap_alloc(heapHandle, sizeof(jnui_label_t));
    layoutElements[bnclElementCount + elementIdx].label = rtData;
    element = jnui_getBnllElement(bnll, elementIdx);
    tileW = ((width - 1) >> 3) + 1;
    tileH = ((height - 1) >> 3) + 1;
    rtData->charDataLength = tileW * tileH * 32;
    rtData->charDataTileOffset = ((int)(vramLeft - 256) >> 5) - tileW * tileH;
    rtData->charData = heap_alloc(heapHandle, rtData->charDataLength);
    NNS_G2dCharCanvasInitForOBJ1D(&rtData->charCanvas, rtData->charData, tileW, tileH, NNS_G2D_CHARA_COLORMODE_16);
    NNS_G2dCharCanvasClear(&rtData->charCanvas, 0);
    NNS_G2dTextCanvasInit(&rtData->textCanvas, &rtData->charCanvas, m2d_getFont(element->font), element->hSpace,
                          element->vSpace);
    rtData->width = width;
    rtData->height = height;
    x = 0;
    y = 0;
    if (parent < bnclElementCount)
    {
        priority = jnui_getCellPriority(parent, cellDataBank, bncl);
        translucent = jnui_getCellIsTranslucent(parent, cellDataBank, bncl);
    }
    else
    {
        priority = jnui_getLabelPriority(parent - bnclElementCount, bnll, layoutElements, bncl);
        translucent = jnui_getLabelIsTranslucent(parent - bnclElementCount, bnll, layoutElements, bncl);
    }
    switch (element->x.origin)
    {
        case JNUI_COORD_H_ORIGIN_CENTER:
            x = -4 * tileW;
            break;
        case JNUI_COORD_H_ORIGIN_RIGHT:
            x = -8 * tileW;
            break;
    }
    switch (element->y.origin)
    {
        case JNUI_COORD_V_ORIGIN_MIDDLE:
            y = -4 * tileH;
            break;
        case JNUI_COORD_V_ORIGIN_BOTTOM:
            y = -8 * tileH;
            break;
    }
    objCount = NNS_G2dArrangeOBJ1D(tmp, tileW, tileH, x, y, GX_OAM_COLORMODE_16, rtData->charDataTileOffset,
                                   NNS_G2D_OBJVRAMMODE_32K);
    cellData = heap_alloc(heapHandle, sizeof(NNSG2dCellDataWithBR) + sizeof(NNSG2dCellOAMAttrData) * objCount);
    cellData->cellData.numOAMAttrs = objCount;
    cellData->cellData.cellAttr = 0;
    cellData->cellData.pOamAttrArray = (NNSG2dCellOAMAttrData*)(cellData + 1);
    cellData->boundingRect.maxX = 4 * tileW;
    cellData->boundingRect.maxY = 4 * tileH;
    cellData->boundingRect.minX = 4 * -tileW;
    cellData->boundingRect.minY = 4 * -tileH;
    for (i = 0; i < objCount; i++)
    {
        G2_SetOBJPriority(&tmp[i], priority);
        if (translucent)
            G2_SetOBJMode(&tmp[i], GX_OAM_MODE_XLU, element->palette);
        else
            G2_SetOBJMode(&tmp[i], GX_OAM_MODE_NORMAL, element->palette);
        G2_OBJMosaic(&tmp[i], FALSE);
        G2_SetOBJEffect(&tmp[i], GX_OAM_EFFECT_NONE, 0);
        cellData->cellData.pOamAttrArray[i].attr0 = tmp[i].attr0;
        cellData->cellData.pOamAttrArray[i].attr1 = tmp[i].attr1;
        cellData->cellData.pOamAttrArray[i].attr2 = tmp[i].attr2;
    }
    rtData->cellData = cellData;
    return rtData->charDataLength;
}

static void jnui_212428C(int engine, int elementIdx, const jnui_bnll_res_t* bnll, const u16* text,
                         const jnui_bncl_res_t* bncl, jnui_layout_element_t* layoutElements, BOOL monospaceNumbers,
                         s8 charWidth)
{
    jnui_label_t* labelData;
    const jnui_bnll_res_element_t* element;
    int width;
    int height;
    int paddedWidth;
    int paddedHeight;
    int alignFlags;
    int x;
    int y;
    element = jnui_getBnllElement(bnll, elementIdx);
    labelData = (layoutElements + jn_ui_getBnclElementCount(bncl) + elementIdx)->label;
    width = labelData->width;
    height = labelData->height;
    paddedWidth = 8 * (((int)(width - 1) >> 3) + 1);
    paddedHeight = 8 * (((int)(height - 1) >> 3) + 1);
    alignFlags = 0;
    x = 0;
    y = 0;
    if (element->x.origin == JNUI_COORD_H_ORIGIN_LEFT)
        alignFlags |= 0x200;
    else if (element->x.origin == JNUI_COORD_H_ORIGIN_CENTER)
    {
        alignFlags |= 0x400;
        x = (int)(paddedWidth - width) >> 1;
    }
    else if (element->x.origin == JNUI_COORD_H_ORIGIN_RIGHT)
    {
        alignFlags |= 0x800;
        x = paddedWidth - width;
    }
    if (element->y.origin == JNUI_COORD_V_ORIGIN_TOP)
        alignFlags |= 0x40;
    else if (element->y.origin == JNUI_COORD_V_ORIGIN_MIDDLE)
    {
        alignFlags |= 0x80;
        y = (int)(paddedHeight - height) >> 1;
    }
    else if (element->y.origin == JNUI_COORD_V_ORIGIN_BOTTOM)
    {
        alignFlags |= 0x100;
        y = paddedHeight - height;
    }
    NNS_G2dCharCanvasClear(&labelData->charCanvas, 0);
    jn_ui_drawFormattedTextRectMonospace(&labelData->textCanvas, x, y, width, height, element->color, alignFlags,
                                         element->x.unk, text, monospaceNumbers, charWidth);
    jnui_registerObjCharVramTransfer(engine, labelData->charDataTileOffset * 32, labelData->charData,
                                     labelData->charDataLength);
}

void jnui_212426C(int engine, int elementIdx, const jnui_bnll_res_t* bnll, const u16* text,
                  const jnui_bncl_res_t* bncl, jnui_layout_element_t* layoutElements)
{
    jnui_212428C(engine, elementIdx, bnll, text, bncl, layoutElements, 0, -1);
}

void jnui_2124248(int engine, int elementIdx, const jnui_bnll_res_t* bnll, const u16* text,
                  const jnui_bncl_res_t* bncl, jnui_layout_element_t* layoutElements, s8 charWidth)
{
    jnui_212428C(engine, elementIdx, bnll, text, bncl, layoutElements, 0, charWidth);
}

void jnui_2124224(int engine, int elementIdx, const jnui_bnll_res_t* bnll, const u16* text,
                  const jnui_bncl_res_t* bncl, jnui_layout_element_t* layoutElements, s8 charWidth)
{
    jnui_212428C(engine, elementIdx, bnll, text, bncl, layoutElements, 1, charWidth);
}

void jnui_21241C0(int engine, int elementIdx, const jnui_bnll_res_t* bnll, const jnui_bncl_res_t* bncl,
                  jnui_layout_element_t* layoutElements)
{
    jnui_getBnllElement(bnll, elementIdx);
    jnui_label_t* labelData = (layoutElements + jn_ui_getBnclElementCount(bncl) + elementIdx)->label;

    NNS_G2dCharCanvasClear(&labelData->charCanvas, 0);
    jnui_registerObjCharVramTransfer(engine, labelData->charDataTileOffset * 32, labelData->charData,
                                     labelData->charDataLength);
}

BOOL jnui_getLabelIsTranslucent(int elementIdx, const jnui_bnll_res_t* bnll, jnui_layout_element_t* layoutElements,
                                const jnui_bncl_res_t* bncl)
{
    const NNSG2dCellDataWithBR* cellData;
    jnui_layout_element_t* element;
    element = layoutElements + elementIdx + jn_ui_getBnclElementCount(bncl);
    cellData = element->label->cellData;
    return ((cellData->cellData.pOamAttrArray->attr0 & GX_OAM_ATTR01_MODE_MASK) >> GX_OAM_ATTR01_MODE_SHIFT) ==
        GX_OAM_MODE_XLU;
}

void jnui_setLabelIsTranslucent(int elementIdx, BOOL translucent, const jnui_bnll_res_t* bnll,
                                jnui_layout_element_t* layoutElements, const jnui_bncl_res_t* bncl)
{
    int i;
    const NNSG2dCellDataWithBR* cellData;
    NNSG2dCellOAMAttrData* pOamData;
    jnui_layout_element_t* element;
    element = layoutElements + elementIdx + jn_ui_getBnclElementCount(bncl);
    cellData = element->label->cellData;
    pOamData = cellData->cellData.pOamAttrArray;
    for (i = 0; i < cellData->cellData.numOAMAttrs; i++)
    {
        pOamData->attr0 &= ~GX_OAM_ATTR01_MODE_MASK;
        if (translucent)
            pOamData->attr0 |= GX_OAM_MODE_XLU << GX_OAM_ATTR01_MODE_SHIFT;
        pOamData++;
    }
}

void jnui_setLabelColorParam(int elementIdx, int colorParam, const jnui_bnll_res_t* bnll,
                             jnui_layout_element_t* layoutElements, const jnui_bncl_res_t* bncl)
{
    int i;
    const NNSG2dCellDataWithBR* cellData;
    NNSG2dCellOAMAttrData* pOamData;
    jnui_layout_element_t* element;
    element = layoutElements + elementIdx + jn_ui_getBnclElementCount(bncl);
    cellData = element->label->cellData;
    pOamData = cellData->cellData.pOamAttrArray;
    for (i = 0; i < cellData->cellData.numOAMAttrs; i++)
    {
        pOamData->attr2 &= ~GX_OAM_ATTR2_CPARAM_MASK;
        pOamData->attr2 |= colorParam << GX_OAM_ATTR2_CPARAM_SHIFT;
        pOamData++;
    }
}

u8 jnui_getLabelPriority(int elementIdx, const jnui_bnll_res_t* bnll, jnui_layout_element_t* layoutElements,
                         const jnui_bncl_res_t* bncl)
{
    const NNSG2dCellDataWithBR* cellData;
    jnui_layout_element_t* element;
    element = layoutElements + elementIdx + jn_ui_getBnclElementCount(bncl);
    cellData = element->label->cellData;
    return (cellData->cellData.pOamAttrArray->attr2 & GX_OAM_ATTR2_PRIORITY_MASK) >> GX_OAM_ATTR2_PRIORITY_SHIFT;
}

void jnui_setLabelPriority(int elementIdx, u8 priority, const jnui_bnll_res_t* bnll,
                           jnui_layout_element_t* layoutElements, const jnui_bncl_res_t* bncl)
{
    int i;
    const NNSG2dCellDataWithBR* cellData;
    NNSG2dCellOAMAttrData* pOamData;
    jnui_layout_element_t* element;
    element = layoutElements + elementIdx + jn_ui_getBnclElementCount(bncl);
    cellData = element->label->cellData;
    pOamData = cellData->cellData.pOamAttrArray;
    for (i = 0; i < cellData->cellData.numOAMAttrs; i++)
    {
        pOamData->attr2 &= ~GX_OAM_ATTR2_PRIORITY_MASK;
        pOamData->attr2 |= priority << GX_OAM_ATTR2_PRIORITY_SHIFT;
        pOamData++;
    }
}

#include <nitro/codereset.h>
