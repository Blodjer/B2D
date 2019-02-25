#pragma once

#ifdef B2D_PLATFORM_WINDOWS
    #define NOMINMAX
    #include <windows.h>
    #undef DELETE
#endif

#include <algorithm>
#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "Core/Core.h"