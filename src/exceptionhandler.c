#include "common.h"
#include "exceptionhandler.h"

static const u8 mDebugFont[] = {
	0x1C, 0x32, 0x2A, 0x2A, 0x26, 0x36, 0x1C, 0x00, 0x08, 0x0C, 0x08, 0x08,
	0x08, 0x08, 0x3C, 0x00, 0x3C, 0x20, 0x20, 0x18, 0x08, 0x04, 0x3C, 0x00,
	0x3C, 0x20, 0x20, 0x18, 0x20, 0x20, 0x1C, 0x00, 0x10, 0x18, 0x14, 0x16,
	0x3E, 0x10, 0x10, 0x00, 0x3C, 0x04, 0x04, 0x38, 0x20, 0x20, 0x1C, 0x00,
	0x1C, 0x06, 0x02, 0x1E, 0x22, 0x22, 0x1C, 0x00, 0x3C, 0x20, 0x10, 0x10,
	0x08, 0x08, 0x08, 0x00, 0x38, 0x24, 0x24, 0x18, 0x24, 0x24, 0x1C, 0x00,
	0x1C, 0x22, 0x22, 0x3C, 0x20, 0x30, 0x1C, 0x00, 0x08, 0x08, 0x14, 0x14,
	0x1C, 0x22, 0x22, 0x00, 0x3C, 0x24, 0x24, 0x1C, 0x24, 0x24, 0x1C, 0x00,
	0x38, 0x0C, 0x04, 0x04, 0x04, 0x0C, 0x38, 0x00, 0x1C, 0x24, 0x24, 0x24,
	0x24, 0x24, 0x1C, 0x00, 0x3C, 0x04, 0x04, 0x1C, 0x04, 0x04, 0x3C, 0x00,
	0x3C, 0x04, 0x04, 0x1C, 0x04, 0x04, 0x04, 0x00
};

void MIi_CpuClear16(u16 data, void *destp, u32 size);

static u16 ExSwizzleDigitPair(u32 val, int pos) {
	val >>= pos;
	val &= 0xFF;
	int n0 = (val & 0xF) << 8;
	int n1 = val >> 4;
	return n1 | n0;
}

static void ExWriteDebug32(u16 *scrPtr, u32 val) {
	scrPtr[0] = ExSwizzleDigitPair(val, 24);
	scrPtr[1] = ExSwizzleDigitPair(val, 16);
	scrPtr[2] = ExSwizzleDigitPair(val, 8);
	scrPtr[3] = ExSwizzleDigitPair(val, 0);
}

#define EXTRA_LINES 4
u32 gExceptionInformation[EXTRA_LINES] = { 0 };

void __attribute__((noreturn)) ExceptionHandler(u32 ptr, void *arg) {
	//when we're running on real hardware, we can not simply print
	//We'll load a simple font and show where the problem arose
	GX_SetBankForBG(GX_VRAM_BG_512_ABCD);
	MI_CpuClearFast((void *) HW_BG_VRAM, 512 * 1024);

	//load in debug font
	static const MIUnpackBitsParam unpackStruct = { 128, 1, 8, 0 }; //128 -> 1024 bytes
	SVC_UnpackBits(mDebugFont, (void *) HW_BG_VRAM, &unpackStruct);
	
	((vu16 *) HW_PLTT)[0] = 0x0017;
	((vu16 *) HW_PLTT)[1] = 0x7FFF;
	GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_1, GX_BG0_AS_2D);
	GX_SetVisiblePlane(GX_PLANEMASK_BG3);
	G2_SetBG3ControlAffine(GX_BG_SCRSIZE_AFFINE_256x256, GX_BG_AREAOVER_XLU, GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x00000);
	
	OSContext *context = (OSContext *) ptr;
	u32 pc = context->pc_plus4;
	u32 lr = context->lr;
	
	//clear canvas
	u16 *canvas = (u16 *) (((u8 *) HW_BG_VRAM) + 0x800);
	MIi_CpuClear16(0x1010, (void *) (HW_BG_VRAM + 0x800), 256 * 256);
	
	//display lr on first row and pc on second row
	ExWriteDebug32(canvas, lr);
	ExWriteDebug32(canvas + 16, pc);

	//stack trace
	int i;
	u32 *sp = (u32 *) context->sp;
	for (i = 0; i < 29; i++) {
		u32 val = sp[i];
		ExWriteDebug32(canvas + 48 + 16 * i, val);
	}
	
	//registers
	for (i = 0; i < 14; i++) {
		u32 val;
		if (i != 13) val = context->r[i];
		if (i == 13) val = context->cpsr;
		ExWriteDebug32(canvas + 48 + 16 * i + 8, val);
	}

	//extra lines
	for (i = 0; i < EXTRA_LINES; i++) {
		u32 val = gExceptionInformation[i];
		ExWriteDebug32(canvas + (24 - EXTRA_LINES + i) * 16 + 8, val);
	}
	
	//asm("mov fp, fp");
	while (1);
}