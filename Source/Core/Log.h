#pragma once

#include "Core/Types/BasicTypes.h"
#include "Core/CoreAssert.h"

#include <spdlog/spdlog.h>

#define STACKLOCATION_INPUT __FILE__, __LINE__, __FUNCSIG__
#define STACKLOCATION_SIGNATURE char const* file, int line, char const* func
#define STACKLOCATION_PARAMS file, line, func
#define STACKLOCATION_LOG_FORMAT "{}:{}\n{}"

class Log
{
public:
    enum class ESink
    {
        Core,
        App
    };

    static constexpr ESink GetSink(char const* file)
    {
        return IsFileInCore(file) ? Log::ESink::Core : Log::ESink::App;
    }

private:
    INLINE static bool m_isInitialized = false;

    INLINE static std::shared_ptr<spdlog::logger> m_loggerCore;
    INLINE static std::shared_ptr<spdlog::logger> m_loggerApp;

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
        GetLogger(file)->warn("Check failed: {}", expr);
        Callstack(spdlog::level::level_enum::warn, STACKLOCATION_PARAMS);
    }

    template<typename... Args>
    FORCEINLINE static void Checkf(STACKLOCATION_SIGNATURE, const char* expr, const char* message, Args const&... args)
    {
        GetLogger(file)->warn("Check failed: {}", expr);
        GetLogger(file)->warn(message, args...);
        Callstack(spdlog::level::level_enum::warn, STACKLOCATION_PARAMS);
    }

    template<typename... Args>
    FORCEINLINE static void Break(STACKLOCATION_SIGNATURE, const char* message, Args const&... args)
    {
        GetLogger(file)->warn(message, args...);
        Callstack(spdlog::level::level_enum::warn, STACKLOCATION_PARAMS);
    }

    FORCEINLINE static void Assert(STACKLOCATION_SIGNATURE, const char* expr)
    {
        GetLogger(file)->critical("Assertion failed: {}", expr);
        Callstack(spdlog::level::level_enum::critical, STACKLOCATION_PARAMS);
    }

    template<typename... Args>
    FORCEINLINE static void Assertf(STACKLOCATION_SIGNATURE, const char* expr, const char* message, Args const&... args)
    {
        GetLogger(file)->critical("Assertion failed: {}", expr);
        GetLogger(file)->critical(message, args...);
        Callstack(spdlog::level::level_enum::critical, STACKLOCATION_PARAMS);
    }

    template<typename... Args>
    FORCEINLINE static void Trap(STACKLOCATION_SIGNATURE, const char* message, Args const&... args)
    {
        GetLogger(file)->critical(message, args...);
        Callstack(spdlog::level::level_enum::critical, STACKLOCATION_PARAMS);
    }

private:
    NOINLINE static void Callstack(spdlog::level::level_enum level, STACKLOCATION_SIGNATURE, uint32 framesToSkip = 0);

    static constexpr spdlog::logger* GetLogger(ESink const sink)
    {
        switch (sink)
        {
        case ESink::Core:
            return m_loggerCore.get();
        case ESink::App:
            return m_loggerApp.get();
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

#define B2D_LOG_INFO(message, ...)		Log::Info(Log::GetSink(__FILE__), message, __VA_ARGS__)
#define B2D_LOG_WARNING(message, ...)	Log::Warning(Log::GetSink(__FILE__), message, __VA_ARGS__)
#define B2D_LOG_ERROR(message, ...)		Log::Error(Log::GetSink(__FILE__), message, __VA_ARGS__)

#else

#define B2D_LOG_INFO(message, ...)
#define B2D_LOG_WARNING(message, ...)
#define B2D_LOG_ERROR(message, ...)

#endif