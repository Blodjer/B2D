#pragma once

#ifdef _DEBUG

#define B2D_DEBUG_MEMORY 1

#if B2D_DEBUG_MEMORY
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>  
#include <crtdbg.h>
#include "Debug/debug_new.cpp"
#define new DEBUG_NEW
#endif

#endif

