#include "common.h"
#include "heap.h"
#include "model.h"
#include "main3d.h"

static NNSG3dGeBuffer* sG3dGeBuffer;
//static camera_t* sCurCamera;
static const MtxFx43* sCurCameraMtx;

void m3d_initResManagement(void)
{
    u16 texBanks = GX_GetBankForTex();
    NNS_GfdInitFrmTexVramManager(
        ((texBanks >> 3) & 1) + ((texBanks >> 2) & 1) + ((texBanks >> 1) & 1) + (texBanks & 1), TRUE);
    NNS_GfdInitFrmPlttVramManager(0x10000, TRUE);
    model_initResList();
}

void m3d_initG3d(void)
{
    NNS_G3dInit();
    NNS_G3dGlbInit();
    NNS_G3dGeReleaseBuffer();
    NNS_G3dGeUseFastDma(TRUE);
    NNS_G3dGlbLightVector(GX_LIGHTID_0, 0, -FX16_ONE, 0);
    NNS_G3dGlbLightVector(GX_LIGHTID_1, FX16_SQRT1_3, FX16_SQRT1_3, FX16_SQRT1_3);
    NNS_G3dGlbLightVector(GX_LIGHTID_2, 0, -FX16_ONE, 0);
    NNS_G3dGlbLightVector(GX_LIGHTID_3, 0, GX_FX16_FX10_MAX, 0);
    NNS_G3dGlbLightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
    NNS_G3dGlbLightColor(GX_LIGHTID_1, GX_RGB(24, 24, 24));
    NNS_G3dGlbLightColor(GX_LIGHTID_2, GX_RGB(31, 0, 0));
    NNS_G3dGlbLightColor(GX_LIGHTID_3, GX_RGB(31, 31, 0));
    NNS_G3dGlbMaterialColorDiffAmb(GX_RGB(20, 20, 20), GX_RGB(20, 20, 20), FALSE);
    NNS_G3dGlbMaterialColorSpecEmi(GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), TRUE);
    NNS_G3dGlbPolygonAttr(GX_LIGHTMASK_0123, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_NONE);
    sG3dGeBuffer = (NNSG3dGeBuffer*)heap_alloc(getSceneHeap(), sizeof(NNSG3dGeBuffer));
    NNS_G3dGeSetBuffer(sG3dGeBuffer);

    NNS_G3dGlbLightVector(GX_LIGHTID_0, FX16_CONST(0.25), 0, -FX16_ONE);
    NNS_G3dGlbLightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
    fx32 t = FX_TRUNC_MUL(
        FX32_CONST(0.1f), FX_Div(FX_SinIdx(FX_DEG_TO_IDX(FX32_CONST(15))), FX_CosIdx(FX_DEG_TO_IDX(FX32_CONST(15)))));
    fx32 b = -t;
    fx32 halfWidth = FX_TRUNC_MUL(FX32_CONST(256.f / 192.f), -t);
    NNS_G3dGlbFrustumW(t, b, halfWidth, -halfWidth, FX32_CONST(0.1f), FX32_CONST(5000), FX32_CONST(0.125));
    //NNS_G3dGlbPerspective(FX32_SIN30, FX32_COS30, FX32_ONE * 4 / 3, FX32_ONE, FX32_ONE * 400);
    NNS_G3dGlbLookAt(
        &(VecFx32){0, 0, FX32_CONST(1000)},
        &(VecFx32){0, FX32_ONE, 0},
        &(VecFx32){0, 0, 0});
    NNS_G3dGlbSetViewPort(0, 0, 255, 191);
    NNS_G3dGlbFlushP();
}

void m3d_render(void)
{
    NNS_G3dGlbFlushP();
    NNS_G3dGeIdentity();
    NNS_G3dGeScale(FX32_ONE, FX32_ONE, FX32_ONE);
    NNS_G3dGeTranslate(0, FX32_CONST(-0.05), FX32_CONST(-2));
    NNS_G3dGeTranslate(0, 0, 0);
}

void m3d_finishRender(void)
{
    NNS_G3dGlbFlushP();
    NNS_G3dGeFlushBuffer();
    G3_SwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_W);
}