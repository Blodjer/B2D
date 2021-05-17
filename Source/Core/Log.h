#pragma once

#include "Core/Types/BasicTypes.h"
#include "Core/CoreAssert.h"

#include <spdlog/spdlog.h>

#define STACKLOCATION_INPUT __FILE__, __LINE__, __FUNCSIG__
#define STACKLOCATION_PARAM_FILE _stacklocation_file
#define STACKLOCATION_PARAM_LINE _stacklocation_line
#define STACKLOCATION_PARAM_FUNC _stacklocation_func
#define STACKLOCATION_PARAMS STACKLOCATION_PARAM_FILE, STACKLOCATION_PARAM_LINE, STACKLOCATION_PARAM_FUNC
#define STACKLOCATION_SIGNATURE char const* STACKLOCATION_PARAM_FILE, int STACKLOCATION_PARAM_LINE, char const* STACKLOCATION_PARAM_FUNC
#define STACKLOCATION_LOG_FORMAT "{}:{}\n{}"

class Log
{
public:
    enum class ESink
    {
        Core,
        App,
        External
    };

    static constexpr ESink GetSink(char const* file)
    {
        return IsFileInCore(file) ? Log::ESink::Core : Log::ESink::App;
    }

private:
    INLINE static bool m_isInitialized = false;

    INLINE static std::shared_ptr<spdlog::logger> m_loggerCore;
    INLINE static std::shared_ptr<spdlog::logger> m_loggerApp;
    INLINE static std::shared_ptr<spdlog::logger> m_loggerExternal;

public:
	static void Init(std::string const& applicationName);

	template<typename... Args>
    FORCEINLINE static void Info(ESink const sink, const char* message, Args const&... args)
	{
        GetLogger(sink)->info(message, args...);
	}

    template<typename... Args>
    FORCEINLINE static void Warning(ESink const sink, const char* message, Args const&... args)
	{
        GetLogger(sink)->warn(message, args...);
	}

    template<typename... Args>
    FORCEINLINE static void Error(ESink const sink, const char* message, Args const&... args)
	{
        GetLogger(sink)->error(message, args...);
	}

    FORCEINLINE static void Check(STACKLOCATION_SIGNATURE, const char* expr)
    {
        GetLogger(STACKLOCATION_PARAM_FILE)->warn("Check failed: {}", expr);
        Callstack(spdlog::level::level_enum::warn, STACKLOCATION_PARAMS);
    }

    template<typename... Args>
    FORCEINLINE static void Checkf(STACKLOCATION_SIGNATURE, const char* expr, const char* message, Args const&... args)
    {
        GetLogger(STACKLOCATION_PARAM_FILE)->warn("Check failed: {}", expr);
        GetLogger(STACKLOCATION_PARAM_FILE)->warn(message, args...);
        Callstack(spdlog::level::level_enum::warn, STACKLOCATION_PARAMS);
    }

    FORCEINLINE static void Assert(STACKLOCATION_SIGNATURE, const char* expr)
    {
        Assert(STACKLOCATION_PARAMS, expr, GetSink(STACKLOCATION_PARAM_FILE));
    }

    FORCEINLINE static void Assert(STACKLOCATION_SIGNATURE, const char* expr, ESink const sink, uint32 framesToSkip = 0)
    {
        GetLogger(sink)->critical("Assertion failed: {}", expr);
        Callstack(sink, spdlog::level::level_enum::critical, STACKLOCATION_PARAMS, framesToSkip);
    }

    template<typename... Args>
    FORCEINLINE static void Assertf(STACKLOCATION_SIGNATURE, const char* expr, const char* message, Args const&... args)
    {
        GetLogger(STACKLOCATION_PARAM_FILE)->critical("Assertion failed: {}", expr);
        GetLogger(STACKLOCATION_PARAM_FILE)->critical(message, args...);
        Callstack(spdlog::level::level_enum::critical, STACKLOCATION_PARAMS);
    }

    template<typename... Args>
    FORCEINLINE static void Trap(STACKLOCATION_SIGNATURE, const char* message, Args const&... args)
    {
        GetLogger(STACKLOCATION_PARAM_FILE)->critical(message, args...);
        Callstack(spdlog::level::level_enum::critical, STACKLOCATION_PARAMS);
    }

    FORCEINLINE static void MissingImplementation(STACKLOCATION_SIGNATURE)
    {
        GetLogger(STACKLOCATION_PARAM_FILE)->critical("Missing implementation: {}", STACKLOCATION_PARAM_FUNC);
    }

private:
    NOINLINE static void Callstack(spdlog::level::level_enum level, STACKLOCATION_SIGNATURE, uint32 framesToSkip = 0);
    NOINLINE static void Callstack(ESink sink, spdlog::level::level_enum level, STACKLOCATION_SIGNATURE, uint32 framesToSkip = 0);

    static constexpr spdlog::logger* GetLogger(ESink const sink)
    {
        switch (sink)
        {
        case ESink::Core:
            return m_loggerCore.get();
        case ESink::App:
            return m_loggerApp.get();
        case ESink::External:
            return m_loggerExternal.get();
        default:
            break;
        }

        B2D_TRAPf("Trying to log to a non-existing sink!");
        return nullptr;
    }

    static constexpr spdlog::logger* GetLogger(char const* file)
    {
        return GetLogger(GetSink(file));
    }

    static constexpr bool IsFileInCore(char const* file)
    {
        std::string_view const string = file; // Could be replaced in C++20
        bool const isInCorePath = string.find("\\B2D\\") != std::string::npos;
        return isInCorePath;
    }
};

#ifndef B2D_NO_LOGGING

#define B2D_LOG_INFO(_message, ...)		        Log::Info(Log::GetSink(__FILE__), _message, __VA_ARGS__)
#define B2D_LOG_WARNING(_message, ...)	        Log::Warning(Log::GetSink(__FILE__), _message, __VA_ARGS__)
#define B2D_LOG_ERROR(_message, ...)	        Log::Error(Log::GetSink(__FILE__), _message, __VA_ARGS__)

#define B2D_LOG_INFO_s(_sink, _message, ...)	Log::Info(_sink, _message, __VA_ARGS__)
#define B2D_LOG_WARNING_s(_sink, _message, ...)	Log::Warning(_sink, _message, __VA_ARGS__)
#define B2D_LOG_ERROR_s(_sink, _message, ...)	Log::Error(_sink, _message, __VA_ARGS__)

#else

#define B2D_LOG_INFO(_message, ...)
#define B2D_LOG_WARNING(_message, ...)
#define B2D_LOG_ERROR(_message, ...)

#define B2D_LOG_INFO_s(_sink, _message, ...)
#define B2D_LOG_WARNING_s(_sink, _message, ...)
#define B2D_LOG_ERROR_s(_sink, _message, ...)

#endif