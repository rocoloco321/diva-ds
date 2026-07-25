#include "common.h"

#include "archive.h"
#include "g2du.h"
#include "globalData.h"
#include "heap.h"
#include "input.h"
#include "jnlib/ui/jnLyt.h"
#include "main2d.h"
#include "main3d.h"
#include "model.h"
#include "oam.h"
#include "scene.h"
#include "print.h"

#include "title.h"

static title_state_t* sTitleSceneState;
static NNSG3dGeBuffer* sG3dGeBuffer;
//model_t model;


static display_config_t dispConfig = 
{
    GX_DISPMODE_GRAPHICS,
    GX_BGMODE_0,
    GX_BG0_AS_3D,
    GX_BGMODE_0,
    GX_VRAM_BG_128_A,
    GX_VRAM_OBJ_NONE,
    GX_VRAM_BGEXTPLTT_NONE,
    GX_VRAM_OBJEXTPLTT_NONE,
    GX_VRAM_TEX_01_CD,
    GX_VRAM_TEXPLTT_0123_E,
    GX_VRAM_CLEARIMAGE_NONE,
    GX_VRAM_SUB_BG_32_H,
    GX_VRAM_SUB_OBJ_16_I,
    GX_VRAM_SUB_BGEXTPLTT_NONE,
    GX_VRAM_SUB_OBJEXTPLTT_NONE,
    GX_VRAM_ARM7_NONE,
    GX_VRAM_LCDC_NONE
};

static int sTitleSceneDests[] = 
{
    SCENE_TITLE,
    SCENE_LOGO,
    SCENE_EXIT
};

void title_init(void)
{
    sTitleSceneState = proc_alloc(sizeof(title_state_t));
    //Setup display config
    m3d_initG3d();
    m2d_loadDisplayConfig(&dispConfig);
    //m2d_setBg0Config(false,1, 0, 0, 1, 0, 0, 0);
    m2d_setBg3Config(false, 3, FALSE, GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1800,
                        GX_BG_CHARBASE_0x00000);
    m2d_setBg2Config(false, 1, FALSE, GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x2000,
                        GX_BG_CHARBASE_0x04000);
    m2d_setBg3Config(true, 3, FALSE, GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                        GX_BG_CHARBASE_0x00000);
    m2d_setBg2Config(true, 2, FALSE, GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1800,
                        GX_BG_CHARBASE_0x00000);
    //Unpack Background data
    g2du_unpackPaletteData(&sTitleSceneState->bgPal.pal, loadFileFast("/data/scene/Title/title_m_b.nclr"));
    g2du_unpackCharacterDataBG(&sTitleSceneState->bg3.chr, loadFileFast("/data/scene/Title/title3_m_b.ncgr"));
    g2du_unpackScreenData(&sTitleSceneState->bg3.scr, loadFileFast("/data/scene/Title/title3_m_b.nscr"));
    g2du_unpackCharacterDataBG(&sTitleSceneState->bg2.chr, loadFileFast("/data/scene/Title/title2_m_b.ncgr"));
    g2du_unpackScreenData(&sTitleSceneState->bg2.scr, loadFileFast("/data/scene/Title/title2_m_b.nscr"));
    g2du_unpackPaletteData(&sTitleSceneState->bgPalSub.pal, loadFileFast("/data/scene/Title/title_s_b.nclr"));
    g2du_unpackCharacterDataBG(&sTitleSceneState->bg3Sub.chr, loadFileFast("/data/scene/Title/title_s_b.ncgr"));
    g2du_unpackScreenData(&sTitleSceneState->bg3Sub.scr, loadFileFast("/data/scene/Title/title3_s_b.nscr"));
    g2du_unpackScreenData(&sTitleSceneState->bg2Sub.scr, loadFileFast("/data/scene/Title/title2_s_b.nscr"));
    
    //Load Backgrounds to VRAM
    GX_LoadBGPltt(sTitleSceneState->bgPal.pal->pRawData, 0, sTitleSceneState->bgPal.pal->szByte);
    GX_LoadBG3Char(sTitleSceneState->bg3.chr->pRawData, 0, sTitleSceneState->bg3.chr->szByte);
    GX_LoadBG3Scr(sTitleSceneState->bg3.scr->rawData, 0, sTitleSceneState->bg3.scr->szByte);
    GX_LoadBG2Char(sTitleSceneState->bg2.chr->pRawData, 0, sTitleSceneState->bg2.chr->szByte);
    GX_LoadBG2Scr(sTitleSceneState->bg2.scr->rawData, 0, sTitleSceneState->bg2.scr->szByte);
    GXS_LoadBGPltt(sTitleSceneState->bgPalSub.pal->pRawData, 0, sTitleSceneState->bgPalSub.pal->szByte);
    GXS_LoadBG3Char(sTitleSceneState->bg3Sub.chr->pRawData, 0, sTitleSceneState->bg3Sub.chr->szByte);
    GXS_LoadBG3Scr(sTitleSceneState->bg3Sub.scr->rawData, 0, sTitleSceneState->bg3Sub.scr->szByte);
    GXS_LoadBG2Scr(sTitleSceneState->bg2Sub.scr->rawData, 0, sTitleSceneState->bg2Sub.scr->szByte);

    //Sub Cell Data
    oam_clearResetBuffers(&sTitleSceneState->mainOam, &sTitleSceneState->subOam);
    g2du_unpackPaletteData(&sTitleSceneState->objSub.pal, loadFileFast("/data/scene/Title/title_s_o.nclr"));
    g2du_unpackCharacterData(&sTitleSceneState->objSub.chr, loadFileFast("/data/scene/Title/title_s_o.ncgr"));
    g2du_unpackCellBankData(&sTitleSceneState->objSub.cellbnk, loadFileFast("/data/scene/Title/title_s.ncer"));
    
    GXS_LoadOBJPltt(sTitleSceneState->objSub.pal->pRawData, 0, sTitleSceneState->objSub.pal->szByte);
    GXS_LoadOBJ(sTitleSceneState->objSub.chr->pRawData, 0, sTitleSceneState->objSub.chr->szByte);
    
    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_32K);

    sTitleSceneState->bnbl = (jnui_bnbl_res_t*)loadFileFast("data/scene/Title/title.bnbl");
    sTitleSceneState->bncl = (jnui_bncl_res_t*)loadFileFast("data/scene/Title/title.bncl");
    jnui_initLayout(getSceneHeap(), sTitleSceneState->bncl, NULL,
                     &sTitleSceneState->layoutElements);

    sTitleSceneState->sel = 0;
    sTitleSceneState->state = 0;
    sTitleSceneState->frameCounter = 0;
    sTitleSceneState->bgOff = 0;
    
    GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3);
    GXS_SetVisiblePlane(GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3 | GX_PLANEMASK_OBJ);
    m2d_setBlendAlpha(GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG3, 8,8);
    m2d_setBlendAlphaSub(GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, 8,8);

    m3d_initResManagement();
    void* nsbmd = loadFileFast("data/actor/actor_00_01.nsbmd");
    void* nsbtx = loadFileFast("data/actor/actor_00_01.nsbtx");
   // DC_StoreRange(nsbmd, ((NNSG3dResFileHeader*)nsbmd)->fileSize);
    model_initFromNsbmdNsbtx(&sTitleSceneState->model, nsbmd, nsbtx);
}

void title_finalize(void)
{
    sTitleSceneState = NULL;
}

static void updateMenu()
{
    for(int i = 0; i<2;i++)
    {
        sTitleSceneState->layoutElements[i].visible = false;
    }
    for(int i=2;i<4;i++)
    {
        sTitleSceneState->layoutElements[i].visible = true;
    }
    int sel = sTitleSceneState->sel;
    sTitleSceneState->layoutElements[sel].visible = true;
    sTitleSceneState->layoutElements[sel+2].visible = false;
}

static void state0_render(scene_manager_t* arg)
{   
    u32 keys_down = input_getPressedKeys();
    if(keys_down & PAD_KEY_UP)
    {
        sTitleSceneState->sel--;
        if(sTitleSceneState->sel<0) sTitleSceneState->sel = 1;
    }
    if(keys_down & PAD_KEY_DOWN)
    {
        sTitleSceneState->sel++;
        if(sTitleSceneState->sel>1) sTitleSceneState->sel = 0;
    }
    if(input_isTouchValid())
    {
        int tpX = input_getPressedTpX();
        int tpY = input_getPressedTpY();
        int touchRes = jn_ui_checkTouch2(tpX, tpY, sTitleSceneState->bnbl);
        if(touchRes != -1)
        {
            sTitleSceneState->sel = touchRes;
            sTitleSceneState->state = 1;
            sTitleSceneState->frameCounter = 0;
        }
    }
    if(keys_down & PAD_BUTTON_A)
    {
        sTitleSceneState->state = 1;  
        sTitleSceneState->frameCounter = 0;
    }
    updateMenu();
}

static void state1_render(scene_manager_t* arg)
{
    int sel = sTitleSceneState->sel;
    if(sTitleSceneState->frameCounter % 3 == 0)
    {
        if(sTitleSceneState->frameCounter & 1)
        {
            sTitleSceneState->layoutElements[sel].visible = true;
            sTitleSceneState->layoutElements[sel+2].visible = false;
        }
        else
        {
            sTitleSceneState->layoutElements[sel].visible = false;
            sTitleSceneState->layoutElements[sel+2].visible = true;    
        }
    }
    if(sTitleSceneState->frameCounter == 60)
    {
        arg->nextScene = sTitleSceneDests[sTitleSceneState->sel];
    }
}

static void render3D()
{
    //model_render(&sTitleSceneState->model);
    NNS_G3dDraw(&sTitleSceneState->model.renderObj);
}

void title_render(scene_manager_t* arg, int frameCounter)
{
    oam_resetBuffers(&sTitleSceneState->mainOam, &sTitleSceneState->subOam);
    sTitleSceneState->frameCounter++;
    switch (sTitleSceneState->state)
    {
    case 0:
        state0_render(arg);
        break;
    case 1:
        state1_render(arg);
        break;
    default:
        break;
    }
    jnui_renderElements(
        sTitleSceneState->bncl,
        sTitleSceneState->objSub.cellbnk,
        NULL,
        &sTitleSceneState->subOam,
        sTitleSceneState->layoutElements);
    //renderCell(&sTitleSceneState->subOam, NNS_G2dGetCellDataByIdx(sTitleSceneState->objSub.cellbnk, 0), 128 << 12, 128 <<12);
    oam_prepareBuffers(&sTitleSceneState->mainOam, &sTitleSceneState->subOam);
    m3d_render();
    render3D();
    m3d_finishRender();
}

void title_vblank(void)
{
    u32 offset = sTitleSceneState->bgOff++;
    G2_SetBG2Offset(-offset>>2, offset>>2);
    G2S_SetBG2Offset(-offset>>2, offset>>2);
    oam_applyBuffers(&sTitleSceneState->mainOam, &sTitleSceneState->subOam);
    /*
    fx32 scroll = FX32_TO_FX32_8(FX32_CONST(0.25));
    bgScrollf(sTitleSceneState->bg2Ptr, -scroll, scroll);
    bgScrollf(sTitleSceneState->bg2PtrSub, -scroll, scroll);
    bgUpdate();*/
}