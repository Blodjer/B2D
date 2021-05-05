#pragma once

// COMPILE = Prevents compilation
// WARNING = Application should be able to handle to issue and continue executing without crash
// ERROR = Application should not crash but is not expected to run functionally
// CRITICAL = Application encountered a major issue and will probably crash

// STATIC ASSERT - COMPILE - Prevents compilation if condition is false
#define B2D_STATIC_ASSERT(_condition, _message) static_assert(_condition, _message);
#define B2D_STATIC_ASSERT_TYPE(_expected, _is)  B2D_STATIC_ASSERT((std::is_base_of<_expected, _is>::value), #_is " is not derived from " #_expected);

// COMPILE ERROR - COMPILE - Prevents compilation
#define B2D_COMPILE_ERROR(_message) static_assert(false, _message);

// MISSING IMPLEMENTATION - COMPILE
#define B2D_NOT_IMPLEMENTED()                   { B2D_LOG_ERROR("Missing Implementation: {}", __FUNCSIG__); throw; }

// BREAK - WARNING - Halts execution in debug configuration
#if B2D_BUILD_DEBUG
#define B2D_BREAK()                         __debugbreak();
#define B2D_BREAK_ON_TRACE(_symbolName)     { if (StackTraceHelper::DoesCurrentStackTraceContainSymbol(#_symbolName)) { B2D_BREAK(); } };
#define B2D_BREAK_ON_TRACE_NOT(_symbolName) { if (!StackTraceHelper::DoesCurrentStackTraceContainSymbol(#_symbolName)) { B2D_BREAK(); } };
#else
#define B2D_BREAK()
#endif

// BREAK - WARNING - Logs warning and halts execution in debug configuration
#define B2D_BREAKf(_message, ...)           { Log::Break(STACKLOCATION_INPUT, _message, __VA_ARGS__); B2D_BREAK(); }

// TRAP - CRITICAL - Halts execution (currently only as breakpoint)
#if B2D_BUILD_DEBUG
#define B2D_TRAP()                          B2D_BREAK(); /*abort();*/
#else
#define B2D_TRAP()                          /*abort();*/
#endif

// TRAP - CRITICAL - Logs critical error and halts execution
#define B2D_TRAPf(_message, ...)            { Log::Trap(STACKLOCATION_INPUT, _message, __VA_ARGS__); B2D_TRAP(); }

// CHECK - WARNING - Logs warning and halts execution in debug configuration if expression is true
#define B2D_CHECK(_expr)                    (!!(_expr) && []() { Log::Check(STACKLOCATION_INPUT, #_expr); B2D_BREAK(); return true; }() )
#define B2D_CHECKf(_expr, _message, ...)    (!!(_expr) && [&]() { Log::Checkf(STACKLOCATION_INPUT, #_expr, _message, __VA_ARGS__); B2D_BREAK(); return true; }() )

// ASSERT - ERROR - Halts execution if expression is false
#define B2D_ASSERT(_expr)                   { if (!(_expr)) { Log::Assert(STACKLOCATION_INPUT, #_expr); B2D_TRAP(); } }
#define B2D_ASSERTl(_expr, _sink)           { if (!(_expr)) { Log::Assert(STACKLOCATION_INPUT, #_expr, _sink); B2D_TRAP(); } }
#define B2D_ASSERTf(_expr, _message, ...)   { if (!(_expr)) { Log::Assertf(STACKLOCATION_INPUT, #_expr, _message, __VA_ARGS__); B2D_TRAP(); } }