#pragma once

#define B2D

#define FORCEINLINE __forceinline

#ifdef _DEBUG
#define B2D_DEBUG
#include "Debug\debug_new.cpp"
#include "Debug.h"
#else
#define B2D_RELEASE
#endif
