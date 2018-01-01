#pragma once

#ifdef _DEBUG

#define DEBUG_MEMORY 1

#if DEBUG_MEMORY
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>  
#include <crtdbg.h>
#include "Debug\debug_new.h"
#define new DEBUG_NEW
#endif

#endif

