#pragma once

#include "Core/Types/BasicTypes.h"

#include <spdlog/spdlog.h>

#define STACK_LOCATION_INPUT __FILE__, __LINE__, __FUNCSIG__
#define STACK_LOCATION_SIGNATURE const char* file, int line, const char* func
#define STACK_LOCATION_PARAMS file, line, func
#define STACK_LOCATION_LOG_FORMAT "{}:{}\n{}"

#define GET_LOGGER_FOR_FILE(file) (::_IsInCore(file) ? Log::GetLoggerCore() : Log::GetLoggerApp())

namespace
{
    // https://stackoverflow.com/questions/10847176/should-i-define-static-inline-methods-in-header-file
    // https://gist.github.com/htfy96/50308afc11678d2e3766a36aa60d5f75
    // Test log cpp function if everything points to one address
    constexpr bool _IsInCore(char const* file)
    {
        std::string_view const string = file; // Could be replaced in C++20
        bool const isInCorePath = string.find("\\B2D\\") != std::string::npos;
        return isInCorePath;
    }
}

class Log
{
private:
	static std::shared_ptr<spdlog::logger> m_loggerCore;
	static std::shared_ptr<spdlog::logger> m_loggerApp;

    INLINE static bool m_isInitialized = false;

public:
	FORCEINLINE static std::shared_ptr<spdlog::logger> GetLoggerCore() { return m_loggerCore; }
    FORCEINLINE static std::shared_ptr<spdlog::logger> GetLoggerApp() { return m_loggerApp; }

	static void Init(std::string const& applicationName);

	template<typename... Args>
    static void Info(std::shared_ptr<spdlog::logger> logger, const char* message, Args const&... args)
	{
		logger->info(message, args...);
	}

	template<typename... Args>
	static void Warning(std::shared_ptr<spdlog::logger> logger, const char* message, Args const&... args)
	{
		logger->warn(message, args...);
	}

	template<typename... Args>
    static void Error(std::shared_ptr<spdlog::logger> logger, const char* message, Args const&... args)
	{
		logger->error(message, args...);
	}

    FORCEINLINE static void Check(STACK_LOCATION_SIGNATURE, const char* expr)
    {
        GET_LOGGER_FOR_FILE(file)->warn("Check failed: {}", expr);
        Callstack(spdlog::level::level_enum::warn, STACK_LOCATION_PARAMS);
    }

    template<typename... Args>
    FORCEINLINE static void Checkf(STACK_LOCATION_SIGNATURE, const char* expr, const char* message, Args const&... args)
    {
        GET_LOGGER_FOR_FILE(file)->warn("Check failed: {}", expr);
        GET_LOGGER_FOR_FILE(file)->warn(message, args...);
        Callstack(spdlog::level::level_enum::warn, STACK_LOCATION_PARAMS);
    }

    template<typename... Args>
    FORCEINLINE static void Break(STACK_LOCATION_SIGNATURE, const char* message, Args const&... args)
    {
        GET_LOGGER_FOR_FILE(file)->warn(message, args...);
        Callstack(spdlog::level::level_enum::warn, STACK_LOCATION_PARAMS);
    }

    FORCEINLINE static void Assert(STACK_LOCATION_SIGNATURE, const char* expr)
    {
        GET_LOGGER_FOR_FILE(file)->critical("Assertion failed: {}", expr);
        Callstack(spdlog::level::level_enum::critical, STACK_LOCATION_PARAMS);
    }

    template<typename... Args>
    FORCEINLINE static void Assertf(STACK_LOCATION_SIGNATURE, const char* expr, const char* message, Args const&... args)
    {
        GET_LOGGER_FOR_FILE(file)->critical("Assertion failed: {}", expr);
        GET_LOGGER_FOR_FILE(file)->critical(message, args...);
        Callstack(spdlog::level::level_enum::critical, STACK_LOCATION_PARAMS);
    }

    template<typename... Args>
    FORCEINLINE static void Trap(STACK_LOCATION_SIGNATURE, const char* message, Args const&... args)
    {
        GET_LOGGER_FOR_FILE(file)->critical(message, args...);
        Callstack(spdlog::level::level_enum::critical, STACK_LOCATION_PARAMS);
    }

private:
    NOINLINE static void Callstack(spdlog::level::level_enum level, STACK_LOCATION_SIGNATURE, uint32 framesToSkip = 0);
};

#ifndef B2D_NO_LOGGING

#define B2D_INFO(message, ...)		Log::Info(GET_LOGGER_FOR_FILE(__FILE__), message, __VA_ARGS__)
#define B2D_WARNING(message, ...)	Log::Warning(GET_LOGGER_FOR_FILE(__FILE__), message, __VA_ARGS__)
#define B2D_ERROR(message, ...)		Log::Error(GET_LOGGER_FOR_FILE(__FILE__), message, __VA_ARGS__)

#else

#define B2D_INFO(message, ...)
#define B2D_WARNING(message, ...)
#define B2D_ERROR(message, ...)

#endif