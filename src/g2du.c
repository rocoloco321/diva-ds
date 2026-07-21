// address=0x020B47D8
#include "common.h"
#include "g2du.h"
//Maybe this is also part of m2d? md2 also has some graphic manipulation functions

void g2du_unpackPaletteData(NNSG2dPaletteData** palData, const void* nclr)
{
    NNSG2dPaletteDataBlock* dataBlock;
    NNSG2dPaletteData* paldata;
    if (nclr)
    {
        dataBlock = (NNSG2dPaletteDataBlock*)NNS_G2dFindBinaryBlock((NNSG2dBinaryFileHeader*)nclr, NNS_G2D_BINBLK_SIG_PALETTEDATA);
        if (dataBlock)
        {
            *palData = &dataBlock->paletteData;
            NNS_G2dUnpackNCL(*palData);
        }
    }
    paldata = *palData;
    DC_FlushRange(paldata->pRawData, paldata->szByte);
    return;
}

void g2du_getPaletteData(NNSG2dPaletteData** palData, const void* nclr)
{
    NNSG2dPaletteDataBlock* dataBlock;
    if (nclr)
    {
        dataBlock = (NNSG2dPaletteDataBlock*)NNS_G2dFindBinaryBlock((NNSG2dBinaryFileHeader*)nclr, NNS_G2D_BINBLK_SIG_PALETTEDATA);
        if (dataBlock)
        {
            *palData = &dataBlock->paletteData;
        }
    }
}

void g2du_unpackCharacterData(NNSG2dCharacterData** charData, const void* ncgr)
{
    NNSG2dCharacterDataBlock* dataBlock;
    NNSG2dCharacterData* chardata;
    if (ncgr)
    {
        dataBlock = (NNSG2dCharacterDataBlock*)NNS_G2dFindBinaryBlock((NNSG2dBinaryFileHeader*)ncgr, NNS_G2D_BINBLK_SIG_CHARACTERDATA);
        if (dataBlock)
        {
            *charData = &dataBlock->characterData;
            NNS_G2dUnpackNCG(*charData);
        }
    }
    chardata = *charData;
    DC_FlushRange(chardata->pRawData, chardata->szByte);
    return;
}

void g2du_getCharacterData(NNSG2dCharacterData** charData, const void* ncgr)
{
    NNSG2dCharacterDataBlock* dataBlock;
    if (ncgr)
    {
        dataBlock = (NNSG2dCharacterDataBlock*)NNS_G2dFindBinaryBlock((NNSG2dBinaryFileHeader*)ncgr, NNS_G2D_BINBLK_SIG_CHARACTERDATA);
        if (dataBlock)
        {
            *charData = &dataBlock->characterData;
        }
    }
}

void g2du_unpackCharacterDataBG(NNSG2dCharacterData** charData, const void* ncgr)
{
    NNSG2dCharacterDataBlock* dataBlock;
    NNSG2dCharacterData* chardata;
    if (ncgr)
    {
        dataBlock = (NNSG2dCharacterDataBlock*)NNS_G2dFindBinaryBlock((NNSG2dBinaryFileHeader*)ncgr, NNS_G2D_BINBLK_SIG_CHARACTERDATA);
        if (dataBlock)
        {
            *charData = &dataBlock->characterData;
            NNS_G2dUnpackBGNCG(*charData);
        }
    }
    chardata = *charData;
    DC_FlushRange(chardata->pRawData, chardata->szByte);
    return;
}

void g2du_getCharacterDataBG(NNSG2dCharacterData** charData, const void* ncgr)
{
    NNSG2dCharacterDataBlock* dataBlock;
    if (ncgr)
    {
        dataBlock = (NNSG2dCharacterDataBlock*)NNS_G2dFindBinaryBlock((NNSG2dBinaryFileHeader*)ncgr, NNS_G2D_BINBLK_SIG_CHARACTERDATA);
        if (dataBlock)
        {
            *charData = &dataBlock->characterData;
        }
    }
}

void g2du_unpackCellBankData(NNSG2dCellDataBank** cellData, const void* ncer)
{
    NNSG2dCellDataBankBlock* dataBlock;
    if (ncer)
    {
        dataBlock = (NNSG2dCellDataBankBlock*)NNS_G2dFindBinaryBlock((NNSG2dBinaryFileHeader*)ncer, NNS_G2D_BLKSIG_CELLBANK);
        if (dataBlock)
        {
            *cellData = &dataBlock->cellDataBank;
            NNS_G2dUnpackNCE(*cellData);
        }
    }
}

void g2du_getCellBankData(NNSG2dCellDataBank** cellData, const void* ncer)
{
    NNSG2dCellDataBankBlock* dataBlock;
    if (ncer)
    {
        dataBlock = (NNSG2dCellDataBankBlock*)NNS_G2dFindBinaryBlock((NNSG2dBinaryFileHeader*)ncer, NNS_G2D_BLKSIG_CELLBANK);
        if (dataBlock)
        {
            *cellData = &dataBlock->cellDataBank;
        }
    }
}

void g2du_unpackAnimBank(NNSG2dAnimBankData** animData, const void* anim)
{
    NNSG2dAnimBankDataBlock* dataBlock;
    if (anim)
    {
        dataBlock = (NNSG2dAnimBankDataBlock*)NNS_G2dFindBinaryBlock((NNSG2dBinaryFileHeader*)anim, NNS_G2D_BLKSIG_ANIMBANK);
        if (dataBlock)
        {
            *animData = &dataBlock->animBankData;
            NNS_G2dUnpackNAN(*animData);
        }
    }
}

void g2du_unpackScreenData(NNSG2dScreenData** scrData, void* nscr)
{
    NNSG2dScreenDataBlock* dataBlock;
    NNSG2dScreenData* screen;
    if (nscr)
    {
        dataBlock = (NNSG2dScreenDataBlock*)NNS_G2dFindBinaryBlock((NNSG2dBinaryFileHeader*)nscr, NNS_G2D_BINBLK_SIG_SCRDATA);
        if (dataBlock)
        {
            *scrData = &dataBlock->screenData;
            NNS_G2dGetUnpackedScreenData(nscr, scrData);
        }
    }
    screen = *scrData;
    DC_FlushRange(screen->rawData, screen->szByte);
}


void g2du_getScreenData(NNSG2dScreenData** scrData, const void* nscr)
{
    NNSG2dScreenDataBlock* dataBlock;
    if (nscr)
    {
        dataBlock = (NNSG2dScreenDataBlock*)NNS_G2dFindBinaryBlock((NNSG2dBinaryFileHeader*)nscr, NNS_G2D_BINBLK_SIG_SCRDATA);
        if (dataBlock)
        {
            *scrData = &dataBlock->screenData;
        }
    }
}
