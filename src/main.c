#include "common.h"

#include "config.h"
#include "exceptionHandler.h"
#include "globalData.h"
#include "heap.h"
#include "input.h"
#include "main2d.h"
#include "main3d.h"
#include "print.h"
#include "saveData.h"
#include "scene.h"
#include "main.h"

#define	DEFAULT_DMA_NUMBER		MI_DMA_MAX_NUM

void NitroStartUp()
{
	OS_Init();
	heap_preInit();
}

static void VBlankIntr()
{
	OS_SetIrqCheckFlag(OS_IE_V_BLANK); // checking V-Blank interrupt
}

static void initSDK()
{
	TPCalibrateParam calibrate;
	OS_SetUserExceptionHandler(ExceptionHandler, NULL);
    OS_InitTick();
    OS_InitAlarm();
    FX_Init();
	TP_Init();
	TP_GetUserInfo(&calibrate);
	TP_SetCalibrateParam(&calibrate);
	GX_SetPower(GX_POWER_ALL);
	GX_Init();
	GX_SetDefaultDMA(2);

	GX_DispOff();
	GXS_DispOff();

    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    OS_EnableIrqMask(OS_IE_V_BLANK);
    OS_EnableIrq();
    OS_EnableInterrupts();
    GX_VBlankIntr(TRUE);

	FS_Init(DEFAULT_DMA_NUMBER);

    CTRDG_Enable(TRUE);

	GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
	MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
	GX_DisableBankForLCDC();

	MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);   // clear OAM
	MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);     // clear the standard palette

	MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);     // clear OAM
	MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);       // clear the standard palette

	SND_Init();
	//NNS_G3dInit();
	m3d_initG3d();
	m3d_initResManagement();
}


void NitroMain()
{
    initSDK();
	NOCASH_Printf("Init SDK ok!\n");
	initHeap();

    OS_WaitVBlankIntr();

	GX_SetMasterBrightness(0);
	GXS_SetMasterBrightness(0);
	GX_DispOn();
	GXS_DispOn();
    NOCASH_Printf("Disps on!\n");
	input_init();
    m2d_initFonts();

    OS_WaitVBlankIntr();

    saveData_init();
	NOCASH_Printf("Running scene_main!\n");
    scene_main();
}

