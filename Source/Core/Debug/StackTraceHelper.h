#pragma once

#include "Core/Types/BasicTypes.h"

class StackTraceHelper
{
public:
    struct StackTraceFrame
    {
        uint64 address;
        std::string module;
        std::string modulePath;
        std::string symbol;
        std::string symbolName;
        std::string file;
        uint32 line;
    };

    struct StackTraceData
    {
        std::vector<StackTraceFrame> frames;
        bool capturedAll = false;
    };

private:
    static bool InitializeSymbolHandler();
    static void CleanupSymbolHandler();

    NOINLINE static bool CaptureStackBacktrace(void** backTraceBuffer, uint32 const maxDepth, uint32 framesToSkip, uint32& outFramesCaptured, bool& outCapturedAll);

public:
    NOINLINE static bool GetStackTrace(StackTraceData& outStackTrace, uint32 framesToSkip = 0, uint32 framesToCapture = DEFAULT_STACK_BACKTRACE_DEPTH);
    NOINLINE static bool GetStackTrace(std::string& outFormatedOutput, uint32 framesToSkip = 0, uint32 framesToCapture = DEFAULT_STACK_BACKTRACE_DEPTH);

    NOINLINE static bool DoesCurrentStackTraceContainSymbol(std::string const& targetSymbol);
    static bool ContainsSymbol(StackTraceData const& stackTrace, std::string const& symbol);

    static void FormatStackTrace(StackTraceData const& stackTrace, std::string& output, bool modules, bool file, bool undecorated);

private:
    static constexpr uint32 DEFAULT_STACK_BACKTRACE_DEPTH = 64;
    static constexpr uint32 MAX_STACK_BACKTRACE_DEPTH = 128;

};
