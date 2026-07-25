#include "common.h"
#include "heap.h"
#include "model.h"
#include "print.h"

static NNSFndList sModelResList;

static BOOL loadTex(NNSG3dResTex* tex);

static void renderG3dDraw1Mat1Shp(model_t* model)
{
    NNS_G3dDraw1Mat1Shp(NNS_G3dRenderObjGetResMdl(&model->renderObj), 0, 0, TRUE);
}

static void renderG3dDraw(model_t* model)
{
    NNS_G3dDraw(&model->renderObj);
}

void (*const gModelRenderFuncs[2])(model_t* model) =
{
    renderG3dDraw,
    renderG3dDraw1Mat1Shp
};

void model_initResList()
{
    NNS_FND_INIT_LIST(&sModelResList, model_res_t, link);
}

static inline void initRes(model_res_t* res)
{
    res->texRes = NULL;
    res->nsbmd = NULL;
    NNS_FndAppendListObject(&sModelResList, res);
}

static inline void setModTex(model_res_t* res, NNSG3dResMdlSet* mdlSet, NNSG3dResTex* tex)
{
    if (!tex)
        return;
    tex->texInfo.flag &= ~NNS_G3D_RESTEX_LOADED;
    tex->tex4x4Info.flag &= ~NNS_G3D_RESTEX4x4_LOADED;
    tex->texInfo.vramKey = 0;
    tex->tex4x4Info.vramKey = 0;
    tex->plttInfo.flag &= ~NNS_G3D_RESPLTT_LOADED;
    tex->plttInfo.vramKey = 0;
    loadTex(tex);
    res->texRes = tex;
    if (mdlSet->header.kind == NNS_G3D_DATABLK_MDLSET)
        NNS_G3dBindMdlSet(mdlSet, tex);
}

void model_resSetNsbmdNsbtx(model_res_t* res, void* nsbmd, void* nsbtx)
{
    NNSG3dResMdlSet* mdlSet = NNS_G3dGetMdlSet(nsbmd);
    NNSG3dResTex* tex = NNS_G3dGetTex(nsbtx);
    initRes(res);
    res->nsbmd = nsbmd;
    setModTex(res, mdlSet, tex);
}

static BOOL loadTex(NNSG3dResTex* tex)
{
    BOOL result = TRUE;
    u32 szTex, szTex4x4, szPltt;
    BOOL sucTex = TRUE;
    BOOL sucTex4x4 = TRUE;
    BOOL sucPltt = TRUE;
    NNSG3dTexKey keyTex;
    NNSG3dTexKey keyTex4x4;
    NNSG3dPlttKey keyPltt;
    if (tex->texInfo.flag & NNS_G3D_RESTEX_LOADED || tex->tex4x4Info.flag & NNS_G3D_RESTEX4x4_LOADED || tex
                                                                                                        ->plttInfo.flag
        & NNS_G3D_RESPLTT_LOADED)
        result = FALSE;
    else
    {
        //this code was in NNS_G3dResDefaultSetup

        szTex = NNS_G3dTexGetRequiredSize(tex);
        szTex4x4 = NNS_G3dTex4x4GetRequiredSize(tex);
        szPltt = NNS_G3dPlttGetRequiredSize(tex);
        if (szTex > 0)
        {
            // If it exists, store in the texture image slot
            keyTex = NNS_GfdAllocTexVram(szTex, FALSE, 0);
            if (keyTex == NNS_GFD_ALLOC_ERROR_TEXKEY)
                sucTex = FALSE;
        }
        else
            keyTex = 0;

        if (szTex4x4 > 0)
        {
            // If it exists, store in the texture image slot
            keyTex4x4 = NNS_GfdAllocTexVram(szTex4x4, TRUE, 0);
            if (keyTex4x4 == NNS_GFD_ALLOC_ERROR_TEXKEY)
                sucTex4x4 = FALSE;
        }
        else
            keyTex4x4 = 0;

        if (szPltt > 0)
        {
            // If it exists, store in the texture palette slot
            keyPltt = NNS_GfdAllocPlttVram(szPltt, tex->tex4x4Info.flag & NNS_G3D_RESPLTT_USEPLTT4, 0);
            if (keyPltt == NNS_GFD_ALLOC_ERROR_PLTTKEY)
                sucPltt = FALSE;
        }
        else
            keyPltt = 0;

        if (!sucTex || !sucTex4x4 || !sucPltt)
        {
            NNS_GfdFreePlttVram(keyPltt);
            NNS_GfdFreeTexVram(keyTex4x4);
            NNS_GfdFreeTexVram(keyTex);
            return FALSE;
        }

        // Key assignments
        NNS_G3dTexSetTexKey(tex, keyTex, keyTex4x4);
        NNS_G3dPlttSetPlttKey(tex, keyPltt);

        // Load to VRAM
        NNS_G3dTexLoad(tex, TRUE);
        NNS_G3dPlttLoad(tex, TRUE);
    }
    return result;
}

static void initModel(model_t* model, NNSG3dResMdl* resMdl, BOOL cullFix)
{
    model->cullReversed = FALSE;
    if (resMdl->info.numMat == 1 && resMdl->info.numShp == 1)
        model->render1Mat1Shp = TRUE;
    else
        model->render1Mat1Shp = FALSE;
    NNS_G3dRenderObjInit(&model->renderObj, resMdl);
    if (!model->cullReversed)
        return;
    u32 i;
    NNSG3dResMdl* resMdl2 = NNS_G3dRenderObjGetResMdl(&model->renderObj);
    for (i = 0; i < resMdl2->info.numMat; i++)
    {
        if (NNS_G3dMdlGetMdlCullMode(resMdl2, i) == GX_CULL_FRONT)
            NNS_G3dMdlSetMdlCullMode(resMdl2, i, GX_CULL_BACK);
        else if (NNS_G3dMdlGetMdlCullMode(resMdl2, i) == GX_CULL_BACK)
            NNS_G3dMdlSetMdlCullMode(resMdl2, i, GX_CULL_FRONT);
    }
}

void model_setCullFront(model_t* model)
{
    u32 i;
    NNSG3dResMdl* resMdl = NNS_G3dRenderObjGetResMdl(&model->renderObj);
    for (i = 0; i < resMdl->info.numMat; i++)
        NNS_G3dMdlSetMdlCullMode(resMdl, i, GX_CULL_FRONT);
}

void model_setCullBack(model_t* model)
{
    u32 i;
    NNSG3dResMdl* resMdl = NNS_G3dRenderObjGetResMdl(&model->renderObj);
    for (i = 0; i < resMdl->info.numMat; i++)
        NNS_G3dMdlSetMdlCullMode(resMdl, i, GX_CULL_BACK);
}

void model_initFromResMdl(model_t* model, NNSG3dResMdl* resMdl)
{
    initModel(model, resMdl, TRUE);
}

NNSG3dResMdl* model_initFromNsbmd(model_t* model, void* nsbmd)
{
    model_resSetNsbmdNsbtx(&model->res, nsbmd, nsbmd);
    NNSG3dResMdlSet* mdlSet = NNS_G3dGetMdlSet(nsbmd);
    initModel(model, NNS_G3dGetMdlByIdx(mdlSet, 0), TRUE);
    return NNS_G3dRenderObjGetResMdl(&model->renderObj);
}

NNSG3dResMdl* model_initFromNsbmdNsbtx(model_t* model, void* nsbmd, void* nsbtx)
{
    model_resSetNsbmdNsbtx(&model->res, nsbmd, nsbtx);
    NNSG3dResMdlSet* mdlSet = NNS_G3dGetMdlSet(nsbmd);
    initModel(model, NNS_G3dGetMdlByIdx(mdlSet, 0), TRUE);
    return NNS_G3dRenderObjGetResMdl(&model->renderObj);
}

NNSG3dResMdl* model_initFromNsbmdNoCullFix(model_t* model, void* nsbmd)
{
    model_resSetNsbmdNsbtx(&model->res, nsbmd, nsbmd);
    NNSG3dResMdlSet* mdlSet = NNS_G3dGetMdlSet(nsbmd);
    initModel(model, NNS_G3dGetMdlByIdx(mdlSet, 0), FALSE);
    return NNS_G3dRenderObjGetResMdl(&model->renderObj);
}

NNSG3dResMdl* model_initFromNsbmdNsbtxMetaCopy(NNSFndHeapHandle heapHandle, model_t* model, void* nsbmd, void* nsbtx)
{
    NNSG3dResTex* resTex;
    u32 size;
    model_res_t* res;
    model_resSetNsbmdNsbtx(&model->res, nsbmd, nsbtx);
    initModel(model, NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(nsbmd), 0), TRUE);
    NNSG3dResMdl* resMdl = NNS_G3dRenderObjGetResMdl(&model->renderObj);
    res = &model->res;
    resTex = res->texRes;
    if (resTex)
    {
        size = resTex->texInfo.ofsTex;
        res->texRes = (NNSG3dResTex*)heap_alloc(heapHandle, size);
        MI_CpuCopy8(resTex, res->texRes, size);
    }
    return resMdl;
}

void model_configShadowPass1(model_t* model, u8 alpha)
{
    NNSG3dResMdl* resMdl = NNS_G3dRenderObjGetResMdl(&model->renderObj);
    GXCull cullMode = model->cullReversed ? GX_CULL_BACK : GX_CULL_FRONT;
    NNS_G3dMdlSetMdlPolygonID(resMdl, 0, 0);
    NNS_G3dMdlSetMdlCullMode(resMdl, 0, cullMode);
    NNS_G3dMdlSetMdlAlpha(resMdl, 0, alpha);
}

void model_configShadowPass2(model_t* model, u8 alpha, u8 polygonId)
{
    NNSG3dResMdl* resMdl = NNS_G3dRenderObjGetResMdl(&model->renderObj);
    GXCull cullMode = model->cullReversed ? GX_CULL_FRONT : GX_CULL_BACK;
    NNS_G3dMdlSetMdlPolygonID(resMdl, 0, polygonId);
    NNS_G3dMdlSetMdlCullMode(resMdl, 0, cullMode);
    NNS_G3dMdlSetMdlAlpha(resMdl, 0, alpha);
}

void model_drawTransformed(model_t* model, const MtxFx43* mtx, const VecFx32* scale)
{
    NNS_G3dGeRestoreMtx(30);
    NNS_G3dGeMultMtx43(mtx);
    if (scale)
        NNS_G3dGeScale(scale->x, scale->y, scale->z);
    model_render(model);
}

void model_render(model_t* model)
{
    gModelRenderFuncs[model->render1Mat1Shp](model);
}

void model_setPolygonId(model_t* model, int polygonId)
{
    NNS_G3dMdlSetMdlPolygonIDAll(NNS_G3dRenderObjGetResMdl(&model->renderObj), polygonId);
}

void model_setLightEnableFlag(model_t* model, int light)
{
    NNS_G3dMdlSetMdlLightEnableFlagAll(NNS_G3dRenderObjGetResMdl(&model->renderObj), light);
}

void model_setEmi(model_t* model, GXRgb col)
{
    NNS_G3dMdlSetMdlEmiAll(NNS_G3dRenderObjGetResMdl(&model->renderObj), col);
}

void model_setPolyIdLightFlagsEmi(model_t* model, int polygonId, int light, GXRgb emi)
{
    NNS_G3dMdlSetMdlPolygonIDAll(NNS_G3dRenderObjGetResMdl(&model->renderObj), polygonId);
    NNS_G3dMdlSetMdlLightEnableFlagAll(NNS_G3dRenderObjGetResMdl(&model->renderObj), light);
    NNS_G3dMdlSetMdlEmiAll(NNS_G3dRenderObjGetResMdl(&model->renderObj), emi);
}

void model_setCullMode(model_t* model, GXCull cullMode)
{
    NNS_G3dMdlSetMdlCullModeAll(NNS_G3dRenderObjGetResMdl(&model->renderObj), cullMode);
}

void model_releaseAll()
{
    model_res_t* mdlRes = (model_res_t*)NNS_FndGetNextListObject(&sModelResList, NULL);
    while (mdlRes)
    {
        if (mdlRes->nsbmd)
            NNS_G3dResDefaultRelease(mdlRes->nsbmd);
        mdlRes = (model_res_t*)NNS_FndGetNextListObject(&sModelResList, mdlRes);
    }
}
