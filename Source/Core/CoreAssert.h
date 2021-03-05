#pragma once

#include "Core/Log.h"
#include "Core/Debug/StackTraceHelper.h"

#define B2D_STATIC_ASSERT(condition, message) static_assert(condition, message);
#define B2D_STATIC_ASSERT_TYPE(expected, is) B2D_STATIC_ASSERT((std::is_base_of<expected, is>::value), #is " is not derived from " #expected);

#if B2D_BUILD_DEBUG
#define B2D_BREAK() __debugbreak();
#define B2D_BREAK_ON_TRACE(symbolName) { if (StackTraceHelper::DoesCurrentStackTraceContainSymbol(#symbolName)) { B2D_BREAK(); } };
#define B2D_BREAK_ON_TRACE_NOT(symbolName) { if (!StackTraceHelper::DoesCurrentStackTraceContainSymbol(#symbolName)) { B2D_BREAK(); } };
#else
#define B2D_BREAK()
#endif

#if B2D_BUILD_DEBUG
#define B2D_TRAP()  __debugbreak(); abort();
#else
#define B2D_TRAP() abort();
#endif

// CHECK - Logs warning and halts execution in debug configuration if expression is true
#define B2D_CHECK(expr)                 (!!(expr) && []() { Log::Check(STACK_LOCATION_INPUT, #expr); B2D_BREAK(); return true; }() )
#define B2D_CHECKf(expr, message, ...)  (!!(expr) && [&]() { Log::Checkf(STACK_LOCATION_INPUT, #expr, message, __VA_ARGS__); B2D_BREAK(); return true; }() )

// BREAK - Logs warning and halts execution in debug configuration
#define B2D_BREAKf(message, ...) { Log::Break(STACK_LOCATION_INPUT, message, __VA_ARGS__); B2D_BREAK(); }

// ASSERT - Halts execution if expression is false
#define B2D_ASSERT(expr)                { if (!(expr)) { Log::Assert(STACK_LOCATION_INPUT, #expr); B2D_TRAP(); } }
#define B2D_ASSERTf(expr, message, ...) { if (!(expr)) { Log::Assertf(STACK_LOCATION_INPUT, #expr, message, __VA_ARGS__); B2D_TRAP(); } }

// TRAP - Halts execution
#define B2D_TRAPf(message, ...)          { Log::Trap(STACK_LOCATION_INPUT, message, __VA_ARGS__); B2D_TRAP(); }