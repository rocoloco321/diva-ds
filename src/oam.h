#pragma once

typedef struct
{
    union
    {
        GXOamAttr oam[128];
        GXOamAffine affine[32];
    };
    u16 objCount;
    u16 affineCount;
} oam_buf_t;

typedef struct
{
    oam_buf_t mainOamBuf;
    oam_buf_t subOamBuf;
} oam_buffers_t;

void oam_clearResetBuffers(oam_buf_t* main, oam_buf_t* sub);
void oam_resetBuffers(oam_buf_t* main, oam_buf_t* sub);
void oam_prepareBuffers(oam_buf_t* main, oam_buf_t* sub);
void oam_applyBuffers(oam_buf_t* main, oam_buf_t* sub);
u16 oam_getAffineCount(oam_buf_t* oamBuf);
void oam_increaseAffineCount(oam_buf_t* oamBuf);
u16 oam_getAffineId(oam_buf_t* oamBuf);

//Similar to G2_SetOBJMode except it only takes the mode parameter.
static inline void oam_setObjMode(GXOamAttr *oam, GXOamMode mode)
{
    oam->attr01 = (oam->attr01 & ~GX_OAM_ATTR01_MODE_MASK) | (mode << GX_OAM_ATTR01_MODE_SHIFT);
}