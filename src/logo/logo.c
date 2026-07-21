#include "common.h"

#include "heap.h"
#include "g2du.h"
#include "main2d.h"
#include "scene.h"
#include "logo.h"

static logo_state_t* sLogoState;

static display_config_t dispConfig = 
{
    GX_DISPMODE_GRAPHICS,
    GX_BGMODE_0,
    FALSE,
    GX_BGMODE_0,
    GX_VRAM_BG_128_A,
    GX_VRAM_OBJ_NONE,
    GX_VRAM_BGEXTPLTT_NONE,
    GX_VRAM_OBJEXTPLTT_NONE,
    GX_VRAM_TEX_NONE,
    GX_VRAM_TEXPLTT_NONE,
    GX_VRAM_CLEARIMAGE_NONE,
    GX_VRAM_SUB_BG_128_C,
    GX_VRAM_SUB_OBJ_NONE,
    GX_VRAM_SUB_BGEXTPLTT_NONE,
    GX_VRAM_SUB_OBJEXTPLTT_NONE,
    GX_VRAM_ARM7_NONE,
    GX_VRAM_LCDC_NONE
};

void logo_init(void)
{
    sLogoState = proc_alloc(sizeof(logo_state_t)); 
    m2d_loadDisplayConfig(&dispConfig);
    m2d_setBg0Config(false, 0, false, GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_256, GX_BG_SCRBASE_0x2000,
                        GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    m2d_setBg0Config(true, 0, false, GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_256, GX_BG_SCRBASE_0x5000,
                        GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    g2du_unpackPaletteData(&sLogoState->bgPal.pal, loadFileFast("/data/scene/Logo/logo_m_b.nclr"));
    g2du_unpackCharacterDataBG(&sLogoState->bg0.chr, loadFileFast("/data/scene/Logo/logo_m_b.ncgr"));
    g2du_unpackScreenData(&sLogoState->bg0.scr, loadFileFast("/data/scene/Logo/logo_m_b.nscr"));
    g2du_unpackPaletteData(&sLogoState->bgPalSub.pal, loadFileFast("/data/scene/Logo/logo_s_b.nclr"));
    g2du_unpackCharacterDataBG(&sLogoState->bg0Sub.chr, loadFileFast("/data/scene/Logo/logo_s_b.ncgr"));
    g2du_unpackScreenData(&sLogoState->bg0Sub.scr, loadFileFast("/data/scene/Logo/logo_s_b.nscr"));

    GX_LoadBGPltt(sLogoState->bgPal.pal->pRawData, 0, sLogoState->bgPal.pal->szByte);
    GX_LoadBG0Char(sLogoState->bg0.chr->pRawData, 0, sLogoState->bg0.chr->szByte);
    GX_LoadBG0Scr(sLogoState->bg0.scr->rawData, 0, sLogoState->bg0.scr->szByte);
    GXS_LoadBGPltt(sLogoState->bgPalSub.pal->pRawData, 0, sLogoState->bgPalSub.pal->szByte);
    GXS_LoadBG0Char(sLogoState->bg0Sub.chr->pRawData, 0, sLogoState->bg0Sub.chr->szByte);
    GXS_LoadBG0Scr(sLogoState->bg0Sub.scr->rawData, 0, sLogoState->bg0Sub.scr->szByte);


    GX_SetVisiblePlane(GX_PLANEMASK_BG0);
    GXS_SetVisiblePlane(GX_PLANEMASK_BG0);
}

void logo_finalize(void)
{
    //m2d_unloadBackground(&sLogoState->bg0);
    //m2d_unloadBackground(&sLogoState->bg0Sub);
    //m2d_unloadPalette(&sLogoState->bgPal);
    //m2d_unloadPalette(&sLogoState->bgPalSub);
    //proc_free(sLogoState);
    sLogoState = NULL;
}

void logo_render(scene_manager_t* arg, int frameCounter)
{
    if(frameCounter == 120)
    {
        arg->nextScene = SCENE_TITLE;
    }
}
