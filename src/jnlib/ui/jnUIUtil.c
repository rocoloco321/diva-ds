// address=0x0201EA30
#include "common.h"
#include "jnLytRes.h"
#include "jnUIUtil.h"

#pragma auto_inline off

int jnui_calcOriginCoordX(jnui_coord_t coord, int width)
{
    int origin;
    int originCoord;
    origin = coord.origin;
    originCoord = coord.coord;
    switch(origin)
    {
        case JNUI_COORD_H_ORIGIN_CENTER:
            originCoord-= (width+1)/2;
            break;
        case JNUI_COORD_H_ORIGIN_RIGHT:
            originCoord -= width;
            break;
    }
    return originCoord;
}

int jnui_calcOriginCoordY(jnui_coord_t coord, int height)
{
    int origin;
    int originCoord;
    origin = coord.origin;
    originCoord = coord.coord;
    switch(origin)
    {
        case JNUI_COORD_V_ORIGIN_MIDDLE:
            originCoord-= (height+1)/2;
            break;
        case JNUI_COORD_V_ORIGIN_BOTTOM:
            originCoord -= height;
            break;
    }
    return originCoord;
}

int jnui_getBnblElementCount(const jnui_bnbl_res_t* bnbl)
{
    return bnbl->header.nrElements;
}

jnui_bnbl_res_element_t* jnui_getBnblElement(jnui_bnbl_res_t* bnbl, int elementId)
{
    jnui_bnbl_res_element_t* elements;
    elements = bnbl->elements;
    if (elementId < 0 || elementId >= (int)jnui_getBnblElementCount(bnbl))
    {
        return NULL;
    }
    else
    {
        return &elements[elementId];
    }
}

int jnui_checkTouch(const jnui_bnbl_res_t* bnbl, int x, int y)
{
    int i;
    const jnui_bnbl_res_element_t* elements;
    int nrElements;
    int minX;
    int minY;
    u8 width;
    u8 height;
    int maxX;
    int maxY;
    elements = bnbl->elements;
    i = 0x0u;
    nrElements = jnui_getBnblElementCount(bnbl);
    if (nrElements > 0)
    {
        do
        {
            minX = jnui_calcOriginCoordX(elements[i].x, elements[i].width);
            minY = jnui_calcOriginCoordY(elements[i].y, elements[i].height);
            width = elements[i].width;
            height = elements[i].height;
            maxX = minX + width;
            maxY = minY + height;
            if (minX <= x && x <= maxX && minY <= y && y <= maxY)
            {
                return i;
            }
            i++;
        }
        while (i < jnui_getBnblElementCount(bnbl));
    }
    return -1;
}

jnui_bnll_res_element_t* jnui_getBnllElement(const jnui_bnll_res_t* bnll, int idx)
{
    const jnui_bnll_res_element_t* var4;
    jnui_bnll_res_element_t* var6;
    int nrElements;
    nrElements = jn_ui_getBnllElementCount(bnll);
    var4 = bnll->elements;
    if (idx < 0 || idx >= nrElements)
    {
        return NULL;
    }
    else
    {
        return (jnui_bnll_res_element_t*)&var4[idx];
    }
}

jnui_bncl_res_element_t* jnui_getBnclElement(const jnui_bncl_res_t* bncl, int idx)
{
    const jnui_bncl_res_element_t* var4;
    jnui_bncl_res_element_t* var6;
    int nrElements;
    nrElements = jn_ui_getBnclElementCount(bncl);
    var4 = bncl->elements;
    if (idx < 0 || idx >= nrElements)
    {
        return NULL;
    }
    else
    {
        return (jnui_bncl_res_element_t*)&var4[idx];
    }
}
