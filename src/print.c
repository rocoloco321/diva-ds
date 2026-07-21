#include "common.h"
#include "print.h"

#include <nitro/code32.h>

static asm void print_asm_end();

static asm void print_asm()
{
	mov r12, r12
	b print_asm_end
}

//This looks like a function, but it actually a trick to allocate room for the
//debug string data
static asm void msgdata()
{
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
}

static asm void print_asm_end()
{
	bx lr
}

#include <nitro/codereset.h>

void NOCASH_Printf(char* msg, ...)
{
	va_list argptr;
	va_start(argptr, msg);

	u8* p = (u8*)&msgdata;
	((u16*)p)[0] = 0x6464;
	((u16*)p)[1] = 0;
	p += 4;
	OS_VSPrintf((char*)p, msg, argptr);
	print_asm();
	va_end(va);
}