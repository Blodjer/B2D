#pragma once

#include "Core/Log.h"

#define B2D_STATIC_ASSERT(condition, message) static_assert(condition, message);
#define B2D_STATIC_ASSERT_TYPE(expected, is) B2D_STATIC_ASSERT((std::is_base_of<expected, is>::value), #is " is not derived from " #expected);

#if B2D_BUILD_DEBUG
#define B2D_BREAK() __debugbreak();
#else
#define B2D_BREAK()
#endif

#ifndef B2D_BUILD_RELEASE
#define B2D_ASSERT(expr) { if (!(expr)) { B2D_LOG_ASSERT(expr); B2D_BREAK(); } }
#else
#define B2D_ASSERT(expr) { if (!(expr)) { B2D_LOG_ASSERT(expr); } }
#endif