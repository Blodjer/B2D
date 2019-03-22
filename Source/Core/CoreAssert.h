#pragma once

#include "Core/Log.h"

#define B2D_STATIC_ASSERT(condition, message) static_assert(condition, message);
#define B2D_STATIC_ASSERT_TYPE(expected, is) B2D_STATIC_ASSERT((std::is_base_of<expected, is>::value), #is " is not derived from " #expected);

#if B2D_BUILD_DEBUG
#define B2D_BREAK() __debugbreak();
#else
#define B2D_BREAK()
#endif

#if B2D_BUILD_DEBUG
#define B2D_ABORT()  __debugbreak(); abort();
#else
#define B2D_ABORT() abort();
#endif

// CHECK - Logs warning and halts execution in debug configuration
#define B2D_CHECK(expr)                 (!!(expr) && []() { Log::Check(CALLSTACK, #expr); B2D_BREAK(); return true; }() )
#define B2D_CHECKf(expr, message, ...)  (!!(expr) && []() { Log::Checkf(CALLSTACK, #expr, message, __VA_ARGS__); B2D_BREAK(); return true; }() )

// ASSERT - Halts execution if expression is false
#define B2D_ASSERT(expr)                { if (!(expr)) { Log::Assert(CALLSTACK, #expr); B2D_ABORT(); } }
#define B2D_ASSERTf(expr, message, ...) { if (!(expr)) { Log::Assertf(CALLSTACK, #expr, message, __VA_ARGS__); B2D_ABORT(); } }

// TRAP - Halts execution
#define B2D_TRAP(message, ...)          { Log::Trap(CALLSTACK, message, __VA_ARGS__); B2D_ABORT(); }