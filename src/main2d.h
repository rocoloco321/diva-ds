#pragma once
#include "common.h"
#include "oam.h"
#include "archive.h"

typedef enum
{
    FONT_CONSOLE,
    FONT_COUNT
} Fonts;

typedef enum
{
    DC_BG23_MODE_TEXT = 1,
    DC_BG23_MODE_AFFINE,
    DC_BG23_MODE_AFFINE_EXT,
    DC_BG23_MODE_256_BMP,
    DC_BG23_MODE_DC_BMP
} DcBg23Mode;

typedef struct
{
    NNSG2dCharacterData* chr;
    NNSG2dScreenData* scr;
}m2d_bg_res_t;

typedef struct 
{
    NNSG2dPaletteData* pal;
}m2d_bgPal_res_t;

typedef struct 
{
    NNSG2dCharacterData* chr;
    NNSG2dPaletteData* pal;
    NNSG2dCellDataBank* cellbnk;
}m2d_obj_res_t;

typedef struct 
{
	u16 mainDisplayMode;
    u16 mainBgMode;
    u16 mainBg03d;
    u16 subBgMode;
    u16 mainBgBank;
    u16 mainObjBank;
    u16 mainBgExtPlttBank;
    u16 mainObjExtPlttBank;
    u16 texBank;
    u16 texPlttBank;
    u16 clearImgBank;
    u16 subBgBank;
    u16 subObjBank;
    u16 subBgExtPlttBank;
    u16 subObjExtPlttBank;
    u16 arm7Bank;
    u16 lcdcBank;
}display_config_t;

//extern m2d_fonts_t gFonts;

void m2d_loadDisplayConfig(display_config_t* config);
void m2d_setBg0Config(BOOL sub, int priority, BOOL mosaic, GXBGScrSizeText screenSize,
                     GXBGColorMode colorMode, GXBGScrBase screenBase, GXBGCharBase charBase, GXBGExtPltt extPlttSlot);
void m2d_setBg1Config(BOOL sub, int priority, BOOL mosaic, GXBGScrSizeText screenSize,
                     GXBGColorMode colorMode, GXBGScrBase screenBase, GXBGCharBase charBase, GXBGExtPltt extPlttSlot);
void m2d_setBg2Config(BOOL sub, int priority, BOOL mosaic, GXBGScrSizeText screenSize,
                     GXBGColorMode colorMode, GXBGScrBase screenBase, GXBGCharBase charBase);
void m2d_setBg3Config(BOOL sub, int priority, BOOL mosaic, GXBGScrSizeText screenSize,
                     GXBGColorMode colorMode, GXBGScrBase screenBase, GXBGCharBase charBase);
void m2d_unloadBackground(m2d_bg_res_t* bg);
void m2d_unloadPalette(m2d_bgPal_res_t* pal);
void m2d_initFonts();
const NNSG2dFont* m2d_getFont(Fonts fontId);


inline void m2d_setBlendAlpha(u32 src, u32 dst, u32 evA, u32 evB)
{
	G2_SetBlendAlpha(src, dst, evA, evB);
}

inline void m2d_setBlendAlphaSub(u32 src, u32 dst, u32 evA, u32 evB)
{
    G2S_SetBlendAlpha(src, dst, evA, evB);
}


inline void m2d_disableBlendAlpha()
{
    G2_BlendNone();
}

static inline void renderCell(oam_buf_t* oamBuf, const NNSG2dCellData* cell, fx32 x, fx32 y)
{
	if(cell == NULL) return;
    NNSG2dFVec2 position = {x, y};
    oamBuf->objCount += NNS_G2dMakeCellToOams(&oamBuf->oam[oamBuf->objCount], 128 - oamBuf->objCount,
                                              cell, NULL, &position, 0, TRUE);
}