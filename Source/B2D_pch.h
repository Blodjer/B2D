#pragma once

#undef GLFW_INCLUDE_VULKAN

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
#include <unordered_set>
#include <vector>
#include <queue>

#include "Core/Core.h"