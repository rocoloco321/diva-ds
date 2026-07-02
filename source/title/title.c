#include <nds.h>
#include <stdlib.h>
#include <NEMain.h>

#include "../archive.h"
#include "../globalData.h"
#include "../math/fx.h"
#include "../ui/btnLyt.h"
#include "../ui/jnLytRes.h"
#include "../main2d.h"
#include "../scene.h"

#include "../sphere_bin.h"
#include "title.h"

static title_state_t* sTitleSceneState;

static display_config_t dispConfig = 
{
    MODE_0_3D,
    MODE_0_2D,
    VRAM_A_MAIN_BG_0x06000000,
    VRAM_B_LCD,
    VRAM_C_LCD,
    VRAM_D_LCD,
    VRAM_E_LCD,
    VRAM_F_LCD,
    VRAM_G_LCD,
    VRAM_H_SUB_BG, 
    VRAM_I_SUB_SPRITE
};

static int sTitleSceneDests[] = 
{
    SCENE_MENU,
    SCENE_MULTIPLAYER,
    SCENE_EDIT,
    SCENE_SETTINGS,
    SCENE_EXIT
};


void title_init(void)
{
    NE_Init3D();
    NE_ClearColorSet(NE_Black, 0, 63);
    sTitleSceneState = malloc(sizeof(title_state_t));
    m2d_loadDisplayConfig(&dispConfig);

    int bg3 = bgInitHidden(3, BgType_Text4bpp, BgSize_T_256x256, 3, 0);
    sTitleSceneState->bg2Ptr = bgInitHidden(2, BgType_Text4bpp, BgSize_T_256x256, 4, 1);
    int bg3Sub = bgInitHiddenSub(3, BgType_Text4bpp, BgSize_T_256x256, 3, 0);
    sTitleSceneState->bg2PtrSub = bgInitHiddenSub(2, BgType_Text4bpp, BgSize_T_256x256, 4, 1);

    m2d_initBackground(&sTitleSceneState->bg3, bg3, "/scene/Title/title3_m_b");
    m2d_initBackground(&sTitleSceneState->bg2, sTitleSceneState->bg2Ptr, "/scene/Title/title2_m_b");
    m2d_initBgPalette(&sTitleSceneState->bgPal, false, "/scene/Title/title_m_b");

    m2d_initBackground(&sTitleSceneState->bg3Sub, bg3Sub, "/scene/Title/title3_s_b");
    m2d_initBackground(&sTitleSceneState->bg2Sub, sTitleSceneState->bg2PtrSub, "/scene/Title/title2_s_b");
    m2d_initBgPalette(&sTitleSceneState->bgPalSub, true, "/scene/Title/title_s_b");

    bgSetPriority(bg3, 3);
    bgSetPriority(sTitleSceneState->bg2Ptr, 1);
    bgShow(bg3);
    bgShow(sTitleSceneState->bg2Ptr);
    m2d_setBlendAlpha(BLEND_SRC_BG2, BLEND_DST_BG3, 8,8);
    bgSetPriority(bg3Sub, 3);
    bgSetPriority(sTitleSceneState->bg2PtrSub, 1);
    bgShow(bg3Sub);
    bgShow(sTitleSceneState->bg2PtrSub);
    m2d_setBlendAlphaSub(BLEND_SRC_BG2, BLEND_DST_BG3, 8,8);

    m2d_clearResetOam();
    m2d_enableOam(true, SpriteMapping_1D_32);
    m2d_loadInitCell(&sTitleSceneState->objSub, true, "/scene/Title/title_s_o");
    sTitleSceneState->bnbl = (jnui_bnbl_res_t*)loadArchive("/scene/Title/title.bnbl");
    sTitleSceneState->bncl = (jnui_bncl_res_t*)loadArchive("/scene/Title/title.bncl");
    sTitleSceneState->lyt = cellLyt_init(sTitleSceneState->bncl);

    sTitleSceneState->sel = 0;
    sTitleSceneState->state = 0;
    sTitleSceneState->frameCounter = 0;
    cellLyt_getLytElement(sTitleSceneState->lyt, 1)->visible = false;
    cellLyt_getLytElement(sTitleSceneState->lyt, 2)->visible = false;
    cellLyt_getLytElement(sTitleSceneState->lyt, 4)->visible = false;
    cellLyt_getLytElement(sTitleSceneState->lyt, 6)->visible = false;
    cellLyt_getLytElement(sTitleSceneState->lyt, 8)->visible = false;
    cellLyt_render(sTitleSceneState->bncl, sTitleSceneState->lyt, sTitleSceneState->objSub.obj, true);
    m2d_applyBuffers();
    m2d_applyBuffers();
    sTitleSceneState->Camera = NE_CameraCreate();
    NE_CameraSet(sTitleSceneState->Camera,
        0, 0, -2,
        0, 0, 0,
        0, 1, 0);
    sTitleSceneState->Sphere = NE_ModelCreate(NE_Static);
    //NE_ModelLoadStaticMeshFAT(sTitleSceneState->Sphere, "/actor/actor_00_00.dsm");
    NE_ModelLoadStaticMesh(sTitleSceneState->Sphere, sphere_bin);
    NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);

}

void title_finalize(void)
{
    m2d_destroyBackground(&sTitleSceneState->bg3);
    m2d_destroyBackground(&sTitleSceneState->bg2);
    m2d_destroyBackground(&sTitleSceneState->bg3Sub);
    m2d_destroyBackground(&sTitleSceneState->bg2Sub);
    m2d_destroyBgPal(&sTitleSceneState->bgPal);
    m2d_destroyBgPal(&sTitleSceneState->bgPalSub);
    m2d_destroyObj(&sTitleSceneState->objSub);
    unloadArchive(sTitleSceneState->bnbl);
    free(sTitleSceneState->lyt);
    free(sTitleSceneState);
    sTitleSceneState = NULL;
}

void updateMenu()
{
    switch (sTitleSceneState->sel)
    {
    case 0:
        cellLyt_getLytElement(sTitleSceneState->lyt, 0)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 1)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 2)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 3)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 4)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 5)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 6)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 7)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 8)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 9)->visible = true;
        break;
    case 1:
        cellLyt_getLytElement(sTitleSceneState->lyt, 0)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 1)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 2)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 3)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 4)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 5)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 6)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 7)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 8)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 9)->visible = true;
        break;
    case 2:
        cellLyt_getLytElement(sTitleSceneState->lyt, 0)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 1)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 2)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 3)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 4)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 5)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 6)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 7)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 8)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 9)->visible = true;
        break;
    case 3:
        cellLyt_getLytElement(sTitleSceneState->lyt, 0)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 1)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 2)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 3)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 4)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 5)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 6)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 7)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 8)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 9)->visible = true;
        break;
    case 4:
        cellLyt_getLytElement(sTitleSceneState->lyt, 0)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 1)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 2)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 3)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 4)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 5)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 6)->visible = false;
        cellLyt_getLytElement(sTitleSceneState->lyt, 7)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 8)->visible = true;
        cellLyt_getLytElement(sTitleSceneState->lyt, 9)->visible = false;
        break;
    default:
        break;
    }
}

void state0_render(scene_manager_t* arg)
{
    scanKeys();
    u32 keys_down = keysDown();
    if(keys_down & KEY_UP)
    {
        sTitleSceneState->sel--;
        if(sTitleSceneState->sel<0) sTitleSceneState->sel = 4;
    }
    if(keys_down & KEY_DOWN)
    {
        sTitleSceneState->sel++;
        if(sTitleSceneState->sel>4) sTitleSceneState->sel = 0;
    }
    if(keys_down & KEY_TOUCH)
    {
        touchPosition tp;
        touchRead(&tp);
        int touchRes = btnLyt_checkTouch(sTitleSceneState->bnbl, tp.px, tp.py);
        if(touchRes != -1)
        {
            sTitleSceneState->sel = touchRes;
            sTitleSceneState->state = 1;
            sTitleSceneState->frameCounter = 0;
        }
    }
    if(keys_down & KEY_A)
    {
        sTitleSceneState->state = 1;  
        sTitleSceneState->frameCounter = 0;
    }
    updateMenu();
}

void state1_render(scene_manager_t* arg)
{
    int elementId = sTitleSceneState->sel << 1;
    if(mod32(sTitleSceneState->frameCounter, 3) == 0)
    {
        if(sTitleSceneState->frameCounter & 1)
        {
            cellLyt_getLytElement(sTitleSceneState->lyt, elementId)->visible = false;
            cellLyt_getLytElement(sTitleSceneState->lyt, elementId+1)->visible = true;
        }
        else
        {
            cellLyt_getLytElement(sTitleSceneState->lyt, elementId)->visible = true;
            cellLyt_getLytElement(sTitleSceneState->lyt, elementId+1)->visible = false;
        }
    }
    if(sTitleSceneState->frameCounter == 60)
    {
        arg->nextScene = sTitleSceneDests[sTitleSceneState->sel];
    }
}

void render3D()
{
    NE_CameraUse(sTitleSceneState->Camera);
    NE_ModelDraw(sTitleSceneState->Sphere);
}

void title_render(scene_manager_t* arg, int frameCounter)
{
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

    m2d_resetOam();
    cellLyt_render(sTitleSceneState->bncl, sTitleSceneState->lyt, sTitleSceneState->objSub.obj, true);
    m2d_prepareBuffers();
    NE_Process(render3D);
}

void title_vblank(void)
{
    fx32 scroll = FX32_TO_FX32_8(FX32_CONST(0.25));
    bgScrollf(sTitleSceneState->bg2Ptr, -scroll, scroll);
    bgScrollf(sTitleSceneState->bg2PtrSub, -scroll, scroll);
    bgUpdate();
    m2d_applyBuffers();
}