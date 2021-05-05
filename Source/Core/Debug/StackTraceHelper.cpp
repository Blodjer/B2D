#include "B2D_pch.h"
#include "StackTraceHelper.h"

#include <dbghelp.h>
#include <sstream>

static std::mutex m_symbolMutex;
static bool m_symbolHandlerInitialized = false;

bool StackTraceHelper::InitializeSymbolHandler()
{
    std::scoped_lock scopedLock(m_symbolMutex);

    if (m_symbolHandlerInitialized)
    {
        return true;
    }

    DWORD options = SymGetOptions();
    options |= SYMOPT_PUBLICS_ONLY;
    options |= SYMOPT_LOAD_LINES;
    options |= SYMOPT_FAIL_CRITICAL_ERRORS;
    options |= SYMOPT_DEFERRED_LOADS;
    options |= SYMOPT_EXACT_SYMBOLS;
    options &= ~SYMOPT_UNDNAME;
    //options |= SYMOPT_UNDNAME;
    SymSetOptions(options);

    if (!SymInitialize(GetCurrentProcess(), nullptr, true))
    {
        HANDLE_LAST_ERROR();
        return false;
    }

    m_symbolHandlerInitialized = true;
    return m_symbolHandlerInitialized;
}

void StackTraceHelper::CleanupSymbolHandler()
{
    std::scoped_lock scopedLock(m_symbolMutex);

    if (!SymCleanup(GetCurrentProcess()))
    {
        HANDLE_LAST_ERROR();
    }
}

bool StackTraceHelper::CaptureStackBacktrace(void** backTraceBuffer, uint32 const maxDepth, uint32 framesToSkip, uint32& outFramesCaptured, bool& outCapturedAll)
{
    outFramesCaptured = 0;
    outCapturedAll = false;

    bool constexpr useFastStackBackTrace = true;
    if constexpr (useFastStackBackTrace)
    {
        framesToSkip += 1;

        outFramesCaptured = ::CaptureStackBackTrace(framesToSkip, maxDepth, backTraceBuffer, nullptr);
        outCapturedAll = outFramesCaptured < maxDepth;

        return true;
    }
    else
    {
        if (!InitializeSymbolHandler())
        {
            return false;
        }

#ifdef B2D_PLATFORM_WINDOWS_X64
        DWORD const machine = IMAGE_FILE_MACHINE_AMD64;
#elif B2D_PLATFORM_WINDOWS_X86
        DWORD const machine = IMAGE_FILE_MACHINE_I386;
#else
        DWORD const machine = IMAGE_FILE_MACHINE_UNKNOWN;
#endif

        HANDLE const process = GetCurrentProcess();
        HANDLE const thread = GetCurrentThread();

        CONTEXT context = {};
        context.ContextFlags = CONTEXT_FULL;
        RtlCaptureContext(&context);

        STACKFRAME64 stk = {};
        memset(&stk, 0, sizeof(STACKFRAME64));
        stk.AddrPC.Mode = AddrModeFlat;
        stk.AddrFrame.Mode = AddrModeFlat;
        stk.AddrStack.Mode = AddrModeFlat;
#ifdef B2D_PLATFORM_WINDOWS_X64
        stk.AddrPC.Offset = context.Rip;
        stk.AddrFrame.Offset = context.Rbp;
        stk.AddrStack.Offset = context.Rsp;
#elif B2D_PLATFORM_WINDOWS_X86
        stk.AddrPC.Offset = context.Eip;
        stk.AddrFrame.Offset = context.Ebp;
        stk.AddrStack.Offset = context.Esp;
#else
        B2D_COMPILE_ERROR("Unkown machine");
#endif

#ifndef B2D_PLATFORM_WINDOWS_X32
        framesToSkip += 1; // For some unknown reason we don't have to skip an addition frame on 32-bit Windows
#endif

        for (uint32 i = 0; i < framesToSkip; ++i)
        {
            if (!StackWalk64(machine, process, thread, &stk, &context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
            {
                outCapturedAll = true;
                return true;
            }
        }
        for (uint32 i = 0; i < maxDepth; ++i)
        {
            if (!StackWalk64(machine, process, thread, &stk, &context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
            {
                outCapturedAll = true;
                break;
            }

            backTraceBuffer[i] = (void*)stk.AddrPC.Offset;
            ++outFramesCaptured;
        }

        return true;
    }

    return false;
}

bool StackTraceHelper::GetStackTrace(StackTraceData& outStackTrace, uint32 framesToSkip, uint32 framesToCapture)
{
    if (framesToCapture == 0)
    {
        framesToCapture = DEFAULT_STACK_BACKTRACE_DEPTH;
    }
    else if (framesToCapture > MAX_STACK_BACKTRACE_DEPTH)
    {
        framesToCapture = MAX_STACK_BACKTRACE_DEPTH;
    }

    framesToSkip += 1;

    void* frames[MAX_STACK_BACKTRACE_DEPTH];
    uint32 framesCaptured = 0;

    if (!CaptureStackBacktrace(frames, framesToCapture, framesToSkip, framesCaptured, outStackTrace.capturedAll))
    {
        return false;
    }

    outStackTrace.frames.reserve(framesCaptured);

    SYMBOL_INFO_PACKAGE symbolpackage = {};
    SYMBOL_INFO& symbol = symbolpackage.si;
    symbol.SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol.MaxNameLen = sizeof(symbolpackage.name);

    IMAGEHLP_LINE64 line = {};
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    DWORD displacement;

    if (!InitializeSymbolHandler())
    {
        return false;
    }

    HANDLE const process = GetCurrentProcess();

    std::scoped_lock lock(m_symbolMutex);
    for (uint32 i = 0; i < framesCaptured; ++i)
    {
        DWORD64 const address = (DWORD64)frames[i];

        StackTraceFrame& frame = outStackTrace.frames.emplace_back();
        frame.address = address;
        
        IMAGEHLP_MODULE64 ImageHelpModule = {};
        ImageHelpModule.SizeOfStruct = sizeof(ImageHelpModule);
        if (SymGetModuleInfo64(process, address, &ImageHelpModule))
        {
            char drive[MAX_PATH];
            char fullpath[MAX_PATH];
            char filename[MAX_PATH];
            char extension[MAX_PATH];
            _splitpath_s(ImageHelpModule.ImageName, drive, sizeof(drive), fullpath, sizeof(fullpath), filename, sizeof(filename), extension, sizeof(extension));
            frame.module = filename;
            frame.module.append(extension);
            frame.modulePath = drive;
            frame.modulePath.append(fullpath);
        }

        if (SymFromAddr(process, address, nullptr, &symbol))
        {
            char undecoratedSymbol[MAX_SYM_NAME];
            UnDecorateSymbolName(symbol.Name, undecoratedSymbol, sizeof(undecoratedSymbol), UNDNAME_NO_ACCESS_SPECIFIERS | UNDNAME_NO_MS_KEYWORDS | UNDNAME_NO_ALLOCATION_MODEL | UNDNAME_NO_ALLOCATION_LANGUAGE);
            frame.symbol = undecoratedSymbol;

            char undecoratedSymbolName[MAX_SYM_NAME];
            UnDecorateSymbolName(symbol.Name, undecoratedSymbolName, sizeof(undecoratedSymbolName), UNDNAME_NAME_ONLY);
            frame.symbolName = undecoratedSymbolName;
        }

        if (SymGetLineFromAddr64(process, address, &displacement, &line))
        {
            frame.file = line.FileName;
            frame.line = line.LineNumber;
        }
    }

    return true;
}

bool StackTraceHelper::GetStackTrace(std::string& outFormatedOutput, uint32 framesToSkip, uint32 framesToCapture)
{
    StackTraceData stackTrace;
    if (!GetStackTrace(stackTrace, framesToSkip + 1, framesToCapture))
    {
        return false;
    }

    FormatStackTrace(stackTrace, outFormatedOutput, false, false, true);
    return true;
}

bool StackTraceHelper::DoesCurrentStackTraceContainSymbol(std::string const& targetSymbol)
{
    StackTraceData stackTrace;
    if (!GetStackTrace(stackTrace, 1))
    {
        return false;
    }

    return ContainsSymbol(stackTrace, targetSymbol);
}

bool StackTraceHelper::ContainsSymbol(StackTraceData const& stackTrace, std::string const& symbol)
{
    for (StackTraceFrame const& frame : stackTrace.frames)
    {
        //if (symbolName.starts_with("\`anonymous namespace\'", 0) == 0 && symbolName.ends_with(targetSymbol))
        //{
        //    return true;
        //}

        if (frame.symbolName == symbol)
        {
            return true;
        }
    }

    return false;
}

void StackTraceHelper::FormatStackTrace(StackTraceData const& stackTrace, std::string& output, bool modules, bool file, bool undecorated)
{
    std::stringstream stream;

    for (uint64 i = 0; i < stackTrace.frames.size(); ++i)
    {
        StackTraceFrame const& frame = stackTrace.frames[i];

        char addressPointer[256] = {};
        snprintf(addressPointer, 256, "0x%016llx", frame.address);

        stream << addressPointer << " " << frame.module.c_str();

        if (!frame.symbolName.empty())
        {
            stream << "!" << frame.symbolName.c_str();
        }

        if (!frame.file.empty())
        {
            stream << " [" << frame.file << ":" << frame.line << "]";
        }

        if (i < (stackTrace.frames.size() - 1))
        {
            stream << "\n";
        }
    }

    if (!stackTrace.capturedAll)
    {
        stream << "[...]\n";
    }

    output = stream.str();
}
