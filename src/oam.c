// address=0x020AE5C4
#include "common.h"
#include "oam.h"
#include "print.h"

#pragma optimize_for_size on

#include <nitro/code16.h>

void oam_clearResetBuffers(oam_buf_t* main, oam_buf_t* sub)
{
    if (main)
    {
        main->objCount = 0;
        main->affineCount = 0;
        MI_DmaFill32(3, main, 0xC0, sizeof(main->oam));
    }
    if (sub)
    {
        sub->objCount = 0;
        sub->affineCount = 0;
        MI_DmaFill32(3, sub, 0xC0, sizeof(sub->oam));
    }
    return;
}

void oam_resetBuffers(oam_buf_t* main, oam_buf_t* sub)
{
    if (main)
    {
        main->objCount = 0;
        main->affineCount = 0;
    }
    if (sub)
    {
        sub->objCount = 0;
        sub->affineCount = 0;
    }
}

void oam_prepareBuffers(oam_buf_t* main, oam_buf_t* sub)
{
    int i;
    int objCount;
    int oamId;
    if (main)
    {
        objCount = main->objCount;
        DC_FlushRange(main, sizeof(main->oam));
        for(i=0; i<128-objCount;i++)
        {
            oamId = objCount+i;
            main->oam[oamId].x = -128;
            main->oam[oamId].y = -32;
        }
    }
    if (sub)
    {
        objCount = sub->objCount;
        DC_FlushRange(sub, sizeof(sub->oam));
        for(i=0; i<128-objCount;i++)
        {
            oamId = objCount+i;
            sub->oam[oamId].x = -128;
            sub->oam[oamId].y = -32;
        }
    }
}

void oam_applyBuffers(oam_buf_t* main, oam_buf_t* sub)
{
    if (main)
    {
        DC_FlushRange(main, sizeof(main->oam));
        GX_LoadOAM(main, 0, sizeof(main->oam));
    }
    if (sub)
    {
        DC_FlushRange(sub, sizeof(sub->oam));
        GXS_LoadOAM(sub, 0, sizeof(sub->oam));
    }
}

u16 oam_getAffineCount(oam_buf_t* oamBuf)
{
    return oamBuf->affineCount;
}

void oam_increaseAffineCount(oam_buf_t* oamBuf)
{
    oamBuf->affineCount++;
}

u16 oam_getAffineId(oam_buf_t* oamBuf)
{
    u16 affineId;
    affineId = oamBuf->affineCount;
    oamBuf->affineCount++;
    return affineId;
}

#include <nitro/codereset.h>