#pragma once
#if __MWERKS__ > 0
#ifdef	__cplusplus
#include "../build/common.mch++"		/* If a compile error occurs in this line, please precompile "Nitro_TS.pch++" */
#else
/* include precompiled header for C source */
#include "../build/common.mch"		/* If a compile error occurs in this line, please precompile "Nitro_TS.pch" */
#endif
#else
#ifdef	__cplusplus
#include "common.pch++"
#else
/* include precompiled header for C source */
#include "common.pch"
#endif
#endif

#define true TRUE
#define false FALSE
#define null NULL

static inline fx32 FX_TRUNC_MUL(fx32 a, fx32 b)
{
    return (s64)a * (s64)b >> FX32_SHIFT;
}