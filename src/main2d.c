#include "common.h"
#include "archive.h"
#include "heap.h"
#include "print.h"

#include "main2d.h"

NNSG2dFont FontArray[FONT_COUNT];

BOOL is3dEnabled;
int bg2Mode;
int bg2ModeSub;
int bg3Mode;
int bg3ModeSub;

void m2d_loadDisplayConfig(display_config_t* config)
{
    MI_CpuFillFast((s32*)0x7000000, 192, 0x400u);
    MI_CpuFillFast((s32*)0x7000400, 192, 0x400u);
    MI_CpuClearFast((s32*)0x5000000, 0x400u);
    MI_CpuClearFast((s32*)0x5000400, 0x400u);
    if (config->mainBgBank)
        GX_SetBankForBG(config->mainBgBank);
    if (config->mainObjBank)
        GX_SetBankForOBJ(config->mainObjBank);
    if (config->mainBgExtPlttBank)
        GX_SetBankForBGExtPltt(config->mainBgExtPlttBank);
    if (config->mainObjExtPlttBank)
        GX_SetBankForOBJExtPltt(config->mainObjExtPlttBank);
    if (config->texBank)
        GX_SetBankForTex(config->texBank);
    if (config->texPlttBank)
        GX_SetBankForTexPltt(config->texPlttBank);
    if (config->clearImgBank)
        GX_SetBankForClearImage(config->clearImgBank);
    if (config->subBgBank)
        GX_SetBankForSubBG(config->subBgBank);
    if (config->subObjBank)
        GX_SetBankForSubOBJ(config->subObjBank);
    if (config->subBgExtPlttBank)
        GX_SetBankForSubBGExtPltt(config->subBgExtPlttBank);
    if (config->subObjExtPlttBank)
        GX_SetBankForSubOBJExtPltt(config->subObjExtPlttBank);
    if (config->arm7Bank)
        GX_SetBankForARM7(config->arm7Bank);
    if (config->lcdcBank)
        GX_SetBankForLCDC(config->lcdcBank);
    GX_SetGraphicsMode(config->mainDisplayMode, config->mainBgMode, config->mainBg03d);
    GXS_SetGraphicsMode(config->subBgMode);

    switch(config->mainBgMode)
    {
        case 0:
            bg2Mode = DC_BG23_MODE_TEXT;
            bg3Mode = DC_BG23_MODE_TEXT;
            break;
        case 1:
            bg2Mode = DC_BG23_MODE_TEXT;
            bg3Mode = DC_BG23_MODE_AFFINE;
            break;
        case 2:
            bg2Mode = DC_BG23_MODE_AFFINE;
            bg3Mode = DC_BG23_MODE_AFFINE;
            break;
        case 3:
            bg2Mode = DC_BG23_MODE_TEXT;
            bg3Mode = DC_BG23_MODE_AFFINE_EXT;
            break;
        case 4:
            bg2Mode = DC_BG23_MODE_AFFINE;
            bg3Mode = DC_BG23_MODE_AFFINE_EXT;
            break;
        case 5:
            bg2Mode = DC_BG23_MODE_AFFINE_EXT;
            bg3Mode = DC_BG23_MODE_AFFINE_EXT;
            break;
        default:
            bg2Mode = 6;
            bg3Mode = 6;
            break;
    }

    switch(config->subBgMode)
    {
        case 0:
            bg2ModeSub = DC_BG23_MODE_TEXT;
            bg3ModeSub = DC_BG23_MODE_TEXT;
            break;
        case 1:
            bg2ModeSub = DC_BG23_MODE_TEXT;
            bg3ModeSub = DC_BG23_MODE_AFFINE;
            break;
        case 2:
            bg2ModeSub = DC_BG23_MODE_AFFINE;
            bg3ModeSub = DC_BG23_MODE_AFFINE;
            break;
        case 3:
            bg2ModeSub = DC_BG23_MODE_TEXT;
            bg3ModeSub = DC_BG23_MODE_AFFINE_EXT;
            break;
        case 4:
            bg2ModeSub = DC_BG23_MODE_AFFINE;
            bg3ModeSub = DC_BG23_MODE_AFFINE_EXT;
            break;
        case 5:
            bg2ModeSub = DC_BG23_MODE_AFFINE_EXT;
            bg3ModeSub = DC_BG23_MODE_AFFINE_EXT;
            break;
        default:
            bg2ModeSub = 6;
            bg3ModeSub = 6;
            break;
    }


    if (config->mainBg03d == TRUE)
    {
        G3X_Init();
        G3X_InitTable();
        G3X_InitMtxStack();
        G3X_SetShading(GX_SHADING_TOON);
        G3X_AntiAlias(TRUE);
        G3X_AlphaTest(TRUE, 0);
        G3X_AlphaBlend(TRUE);
        G2_BlendNone();
        G3_SwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_W);
        G3X_SetClearColor(
            0x7FFF,
            31,
            0x7FFF, 63, 0);
    }
    is3dEnabled = config->mainBg03d;
    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
}

void m2d_setBg0Config(BOOL sub, int priority, BOOL mosaic, GXBGScrSizeText screenSize,
                     GXBGColorMode colorMode, GXBGScrBase screenBase, GXBGCharBase charBase, GXBGExtPltt extPlttSlot)
{
    if(!sub)
    {
        G2_SetBG0Priority(priority);
        G2_BG0Mosaic(mosaic);
        if(is3dEnabled)
        {
            G3X_SetHOffset(0);
        }
        else
        {
            G2_SetBG0Offset(0, 0);
            G2_SetBG0Control(screenSize, colorMode, screenBase, charBase, extPlttSlot);
        }
    }
    else
    {
        G2S_SetBG0Offset(0, 0);
        G2S_SetBG0Priority(priority);
        G2S_BG0Mosaic(mosaic);
        G2S_SetBG0Control(screenSize, colorMode, screenBase, charBase, extPlttSlot);
    }
}

void m2d_setBg1Config(BOOL sub, int priority, BOOL mosaic, GXBGScrSizeText screenSize,
                     GXBGColorMode colorMode, GXBGScrBase screenBase, GXBGCharBase charBase, GXBGExtPltt extPlttSlot)
{
    if(!sub)
    {
        G2_SetBG1Offset(0, 0);
        G2_SetBG1Priority(priority);
        G2_BG1Mosaic(mosaic);
        G2_SetBG1Control(screenSize, colorMode, screenBase, charBase, extPlttSlot);
    }
    else
    {
        G2S_SetBG1Offset(0, 0);
        G2S_SetBG1Priority(priority);
        G2S_BG1Mosaic(mosaic);
        G2S_SetBG1Control(screenSize, colorMode, screenBase, charBase, extPlttSlot);
    }
}

void m2d_setBg2Config(BOOL sub, int priority, BOOL mosaic, GXBGScrSizeText screenSize,
                     GXBGColorMode colorMode, GXBGScrBase screenBase, GXBGCharBase charBase)
{
    MtxFx22 affineMtx;

    affineMtx._01 = 0;
    affineMtx._10 = 0;
    affineMtx._00 = FX32_ONE;
    affineMtx._11 = FX32_ONE;
    int bgMode = !sub ? bg2Mode : bg2ModeSub;
    switch (bgMode)
    {
        case DC_BG23_MODE_TEXT:
            if (!sub)
            {
                G2_SetBG2Offset(0, 0);
                G2_SetBG2Priority(priority);
                G2_BG2Mosaic(mosaic);
                G2_SetBG2ControlText(screenSize, colorMode, screenBase, charBase);
            }
            else
            {
                G2S_SetBG2Offset(0, 0);
                G2S_SetBG2Priority(priority);
                G2S_BG2Mosaic(mosaic);
                G2S_SetBG2ControlText(screenSize, colorMode, screenBase, charBase);
            }
            break;
        case DC_BG23_MODE_AFFINE:
            if (!sub)
            {
                G2_SetBG2Affine(&affineMtx, 0, 0, 0, 0);
                G2_SetBG2Priority(priority);
                G2_BG2Mosaic(mosaic);
                G2_SetBG2ControlAffine(screenSize, colorMode, screenBase,
                                    charBase);
            }
            else
            {
                G2S_SetBG2Affine(&affineMtx, 0, 0, 0, 0);
                G2S_SetBG2Priority(priority);
                G2S_BG2Mosaic(mosaic);
                G2S_SetBG2ControlAffine(screenSize, colorMode, screenBase,
                                        charBase);
            }
            break;
        case DC_BG23_MODE_AFFINE_EXT:
            if (!sub)
            {
                G2_SetBG2Affine(&affineMtx, 0, 0, 0, 0);
                G2_SetBG2Priority(priority);
                G2_BG2Mosaic(mosaic);
                G2_SetBG2Control256x16Pltt(screenSize, colorMode, screenBase, 0);
            }
            else
            {
                G2S_SetBG2Affine(&affineMtx, 0, 0, 0, 0);
                G2S_SetBG2Priority(priority);
                G2S_BG2Mosaic(mosaic);
                G2S_SetBG2Control256x16Pltt(screenSize, colorMode, screenBase, 0);
            }
            break;
        case DC_BG23_MODE_256_BMP:
            if (!sub)
            {
                G2_SetBG2Affine(&affineMtx, 0, 0, 0, 0);
                G2_SetBG2Priority(priority);
                G2_BG2Mosaic(mosaic);
                G2_SetBG2Control256Bmp(screenSize, colorMode, screenBase);
            }
            else
            {
                G2S_SetBG2Affine(&affineMtx, 0, 0, 0, 0);
                G2S_SetBG2Priority(priority);
                G2S_BG2Mosaic(mosaic);
                G2S_SetBG2Control256Bmp(screenSize, colorMode, screenBase);
            }
            break;
        case DC_BG23_MODE_DC_BMP:
            if (!sub)
            {
                G2_SetBG2Affine(&affineMtx, 0, 0, 0, 0);
                G2_SetBG2Priority(priority);
                G2_BG2Mosaic(mosaic);
                G2_SetBG2ControlDCBmp(screenSize, colorMode, screenBase);
            }
            else
            {
                G2S_SetBG2Affine(&affineMtx, 0, 0, 0, 0);
                G2S_SetBG2Priority(priority);
                G2S_BG2Mosaic(mosaic);
                G2S_SetBG2ControlDCBmp(screenSize, colorMode, screenBase);
            }
            break;
    }
}

void m2d_setBg3Config(BOOL sub, int priority, BOOL mosaic, GXBGScrSizeText screenSize,
                     GXBGColorMode colorMode, GXBGScrBase screenBase, GXBGCharBase charBase)
{
    MtxFx22 affineMtx;

    affineMtx._01 = 0;
    affineMtx._10 = 0;
    affineMtx._00 = FX32_ONE;
    affineMtx._11 = FX32_ONE;
    int bgMode = !sub ? bg3Mode : bg3ModeSub;
    switch (bgMode)
    {
        case DC_BG23_MODE_TEXT:
            if (!sub)
            {
                G2_SetBG3Offset(0, 0);
                G2_SetBG3Priority(priority);
                G2_BG3Mosaic(mosaic);
                G2_SetBG3ControlText(screenSize, colorMode, screenBase, charBase);
            }
            else
            {
                G2S_SetBG3Offset(0, 0);
                G2S_SetBG3Priority(priority);
                G2S_BG3Mosaic(mosaic);
                G2S_SetBG3ControlText(screenSize, colorMode, screenBase, charBase);
            }
            break;
        case DC_BG23_MODE_AFFINE:
            if (!sub)
            {
                G2_SetBG3Affine(&affineMtx, 0, 0, 0, 0);
                G2_SetBG3Priority(priority);
                G2_BG3Mosaic(mosaic);
                G2_SetBG3ControlAffine(screenSize, colorMode, screenBase,
                                       charBase);
            }
            else
            {
                G2S_SetBG3Affine(&affineMtx, 0, 0, 0, 0);
                G2S_SetBG3Priority(priority);
                G2S_BG3Mosaic(mosaic);
                G2S_SetBG3ControlAffine(screenSize, colorMode, screenBase,
                                        charBase);
            }
            break;
        case DC_BG23_MODE_AFFINE_EXT:
            if (!sub)
            {
                G2_SetBG3Affine(&affineMtx, 0, 0, 0, 0);
                G2_SetBG3Priority(priority);
                G2_BG3Mosaic(mosaic);
                G2_SetBG3Control256x16Pltt(screenSize, colorMode, screenBase, 0);
            }
            else
            {
                G2S_SetBG3Affine(&affineMtx, 0, 0, 0, 0);
                G2S_SetBG3Priority(priority);
                G2S_BG3Mosaic(mosaic);
                G2S_SetBG3Control256x16Pltt(screenSize, colorMode, screenBase, 0);
            }
            break;
        case DC_BG23_MODE_256_BMP:
            if (!sub)
            {
                G2_SetBG3Affine(&affineMtx, 0, 0, 0, 0);
                G2_SetBG3Priority(priority);
                G2_BG3Mosaic(mosaic);
                G2_SetBG3Control256Bmp(screenSize, colorMode, screenBase);
            }
            else
            {
                G2S_SetBG3Affine(&affineMtx, 0, 0, 0, 0);
                G2S_SetBG3Priority(priority);
                G2S_BG3Mosaic(mosaic);
                G2S_SetBG3Control256Bmp(screenSize, colorMode, screenBase);
            }
            break;
        case DC_BG23_MODE_DC_BMP:
            if (!sub)
            {
                G2_SetBG3Affine(&affineMtx, 0, 0, 0, 0);
                G2_SetBG3Priority(priority);
                G2_BG3Mosaic(mosaic);
                G2_SetBG3ControlDCBmp(screenSize, colorMode, screenBase);
            }
            else
            {
                G2S_SetBG3Affine(&affineMtx, 0, 0, 0, 0);
                G2S_SetBG3Priority(priority);
                G2S_BG3Mosaic(mosaic);
                G2S_SetBG3ControlDCBmp(screenSize, colorMode, screenBase);
            }
            break;
    }
}

void m2d_initFonts()
{
    void * file = loadFileFast("/data/LC_Font_s.NFTR");
    if(file == NULL)
    {
        NOCASH_Printf("File load has failed\n");
    }
    NNS_G2dFontInitAuto(&FontArray[0], file);
    NOCASH_Printf("InitFonts Success!\n");
}

const NNSG2dFont* m2d_getFont(Fonts fontId)
{
    return &FontArray[fontId];
}