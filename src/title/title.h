#pragma once
#include "jnlib.h"
#include "main2d.h"
#include "model.h"
#include "oam.h"
typedef struct
{
    m2d_bg_res_t bg3;
    m2d_bg_res_t bg2;
    m2d_bg_res_t bg3Sub;
    m2d_bg_res_t bg2Sub;
    m2d_bgPal_res_t bgPal;
    m2d_bgPal_res_t bgPalSub;
    m2d_obj_res_t objSub;
    jnui_layout_element_t* layoutElements;
    jnui_bnbl_res_t* bnbl;
    jnui_bncl_res_t* bncl;
    model_t model;
    oam_buf_t mainOam;
    oam_buf_t subOam;
    int state;
    u32 frameCounter;
    u32 bgOff;
    int sel;
} title_state_t;


void title_init(void);
void title_finalize(void);
void title_render(scene_manager_t* arg, int frameCounter);
void title_vblank(void);