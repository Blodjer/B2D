#pragma once

// COMPILE = Prevents compilation
// WARNING = Application should be able to handle to issue and continue executing without crash
// ERROR = Application should not crash but is not expected to run functionally
// CRITICAL = Application encountered a major issue and will probably crash

// STATIC ASSERT - COMPILE - Prevents compilation if condition is false
#define B2D_STATIC_ASSERT(condition, message) static_assert(condition, message);
#define B2D_STATIC_ASSERT_TYPE(expected, is) B2D_STATIC_ASSERT((std::is_base_of<expected, is>::value), #is " is not derived from " #expected);

// MISSING IMPLEMENTATION - COMPILE
#define B2D_NOT_IMPLEMENTED() { B2D_LOG_ERROR("Missing Implementation: {}", __FUNCSIG__); throw; }

// BREAK - WARNING - Halts execution in debug configuration
#if B2D_BUILD_DEBUG
#define B2D_BREAK() __debugbreak();
#define B2D_BREAK_ON_TRACE(symbolName) { if (StackTraceHelper::DoesCurrentStackTraceContainSymbol(#symbolName)) { B2D_BREAK(); } };
#define B2D_BREAK_ON_TRACE_NOT(symbolName) { if (!StackTraceHelper::DoesCurrentStackTraceContainSymbol(#symbolName)) { B2D_BREAK(); } };
#else
#define B2D_BREAK()
#endif

// BREAK - WARNING - Logs warning and halts execution in debug configuration
#define B2D_BREAKf(message, ...) { Log::Break(STACKLOCATION_INPUT, message, __VA_ARGS__); B2D_BREAK(); }

// TRAP - CRITICAL - Halts execution (currently only as breakpoint)
#if B2D_BUILD_DEBUG
#define B2D_TRAP() B2D_BREAK(); /*abort();*/
#else
#define B2D_TRAP() /*abort();*/
#endif

// TRAP - CRITICAL - Logs critical error and halts execution
#define B2D_TRAPf(message, ...)          { Log::Trap(STACKLOCATION_INPUT, message, __VA_ARGS__); B2D_TRAP(); }

// CHECK - WARNING - Logs warning and halts execution in debug configuration if expression is true
#define B2D_CHECK(expr)                 (!!(expr) && []() { Log::Check(STACKLOCATION_INPUT, #expr); B2D_BREAK(); return true; }() )
#define B2D_CHECKf(expr, message, ...)  (!!(expr) && [&]() { Log::Checkf(STACKLOCATION_INPUT, #expr, message, __VA_ARGS__); B2D_BREAK(); return true; }() )

// ASSERT - ERROR - Halts execution if expression is false
#define B2D_ASSERT(expr)                { if (!(expr)) { Log::Assert(STACKLOCATION_INPUT, #expr); B2D_TRAP(); } }
#define B2D_ASSERTf(expr, message, ...) { if (!(expr)) { Log::Assertf(STACKLOCATION_INPUT, #expr, message, __VA_ARGS__); B2D_TRAP(); } }