#pragma once

#include <spdlog/spdlog.h>

#define CALLSTACK __FILE__, __LINE__, __FUNCSIG__
#define CALLSTACK_SIGNATURE const char* file, int line, const char* func
#define CALLSTACK_PARAMS file, line, func
#define CALLSTACK_LOG_FORMAT "{}:{} {}"

class Log
{
private:
	static std::shared_ptr<spdlog::logger> m_loggerCore;
	static std::shared_ptr<spdlog::logger> m_loggerApp;

public:
	FORCEINLINE static std::shared_ptr<spdlog::logger> GetLoggerCore() { return m_loggerCore; }
    FORCEINLINE static std::shared_ptr<spdlog::logger> GetLoggerApp() { return m_loggerApp; }

	static void Init(std::string const& applicationName);

	template<typename... Args>
    INLINE static void Info(std::shared_ptr<spdlog::logger> logger, const char* message, Args const&... args)
	{
		logger->info(message, args...);
	}

	template<typename... Args>
	INLINE static void Warning(std::shared_ptr<spdlog::logger> logger, const char* message, Args const&... args)
	{
		logger->warn(message, args...);
	}

	template<typename... Args>
    INLINE static void Error(std::shared_ptr<spdlog::logger> logger, const char* message, Args const&... args)
	{
		logger->error(message, args...);
	}

    INLINE static void Check(CALLSTACK_SIGNATURE, const char* expr)
    {
        m_loggerCore->warn("Check failed: ({})", expr);
        Callstack(spdlog::level::level_enum::warn, CALLSTACK_PARAMS);
    }

    template<typename... Args>
    INLINE static void Checkf(CALLSTACK_SIGNATURE, const char* expr, const char* message, Args const&... args)
    {
        m_loggerCore->warn("Check failed: ({})", expr);
        m_loggerCore->warn(message, args...);
        Callstack(spdlog::level::level_enum::warn, CALLSTACK_PARAMS);
    }

    template<typename... Args>
    INLINE static void Break(CALLSTACK_SIGNATURE, const char* message, Args const&... args)
    {
        m_loggerCore->warn(message, args...);
        Callstack(spdlog::level::level_enum::warn, CALLSTACK_PARAMS);
    }

    INLINE static void Assert(CALLSTACK_SIGNATURE, const char* expr)
    {
        m_loggerCore->critical("Assertion failed: ({})", expr);
        Callstack(spdlog::level::level_enum::critical, CALLSTACK_PARAMS);
    }

    template<typename... Args>
    INLINE static void Assertf(CALLSTACK_SIGNATURE, const char* expr, const char* message, Args const&... args)
    {
        m_loggerCore->critical("Assertion failed: ({})", expr);
        m_loggerCore->critical(message, args...);
        Callstack(spdlog::level::level_enum::critical, CALLSTACK_PARAMS);
    }

    template<typename... Args>
    INLINE static void Trap(CALLSTACK_SIGNATURE, const char* message, Args const&... args)
    {
        m_loggerCore->critical(message, args...);
        Callstack(spdlog::level::level_enum::critical, CALLSTACK_PARAMS);
    }

private:
    INLINE static void Callstack(spdlog::level::level_enum level, CALLSTACK_SIGNATURE)
    {
        m_loggerCore->log(level, CALLSTACK_LOG_FORMAT, CALLSTACK_PARAMS);
    }
};

#ifndef B2D_NO_LOGGING

#define B2D_CORE_INFO(message, ...)		Log::Info(Log::GetLoggerCore(), message, __VA_ARGS__)
#define B2D_CORE_WARNING(message, ...)	Log::Warning(Log::GetLoggerCore(), message, __VA_ARGS__)
#define B2D_CORE_ERROR(message, ...)	Log::Error(Log::GetLoggerCore(), message, __VA_ARGS__)

#define B2D_INFO(message, ...)			Log::Info(Log::GetLoggerApp(), message, __VA_ARGS__)
#define B2D_WARNING(message, ...)		Log::Warning(Log::GetLoggerApp(), message, __VA_ARGS__)
#define B2D_ERROR(message, ...)			Log::Error(Log::GetLoggerApp(), message, __VA_ARGS__)

#else

#define B2D_CORE_INFO(message, ...)
#define B2D_CORE_WARNING(message, ...)
#define B2D_CORE_ERROR(message, ...)

#define B2D_INFO(message, ...)
#define B2D_WARNING(message, ...)
#define B2D_ERROR(message, ...)

#endif