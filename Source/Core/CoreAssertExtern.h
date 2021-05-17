#pragma once

// DO NOT INCLUDE ANYTHING HERE. THIS FILE WILL BE USED BY EXTERNAL LIBRARIES.

namespace B2D_Extern
{
    extern void Assert(char const*, int, char const*, char const*);
}

#ifndef B2D

#if B2D_BUILD_DEBUG
#define B2D_BREAK_EXTERN() __debugbreak();
#else
#define B2D_BREAK_EXTERN()
#endif

#define B2D_ASSERT_EXTERN(_expr) do { if (!(_expr)) { B2D_Extern::Assert(__FILE__, __LINE__, __FUNCSIG__, #_expr); B2D_BREAK_EXTERN(); } } while(0)

#endif