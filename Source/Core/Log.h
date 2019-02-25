#pragma once

#include <spdlog/spdlog.h>

class Log
{
private:
	static std::shared_ptr<spdlog::logger> mLoggerCore;
	static std::shared_ptr<spdlog::logger> mLoggerApp;

public:
	FORCEINLINE static std::shared_ptr<spdlog::logger> GetLoggerCore() { return mLoggerCore; }
    FORCEINLINE static std::shared_ptr<spdlog::logger> GetLoggerApp() { return mLoggerApp; }

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

    INLINE static void Assert(const char* expr, const char* file, int line)
    {
        mLoggerCore->critical("Assertion failed: ({0}) in {1}:{2}", expr, file, line);
    }
};

#ifndef B2D_NO_LOGGING

#define B2D_CORE_INFO(message, ...)		Log::Info(Log::GetLoggerCore(), message, __VA_ARGS__)
#define B2D_CORE_WARNING(message, ...)	Log::Warning(Log::GetLoggerCore(), message, __VA_ARGS__)
#define B2D_CORE_ERROR(message, ...)	Log::Error(Log::GetLoggerCore(), message, __VA_ARGS__)

#define B2D_INFO(message, ...)			Log::Info(Log::GetLoggerApp(), message, __VA_ARGS__)
#define B2D_WARNING(message, ...)		Log::Warning(Log::GetLoggerApp(), message, __VA_ARGS__)
#define B2D_ERROR(message, ...)			Log::Error(Log::GetLoggerApp(), message, __VA_ARGS__)

#define B2D_LOG_ASSERT(expr)	        Log::Assert(#expr, __FILE__, __LINE__)

#else

#define B2D_CORE_INFO(message, ...)
#define B2D_CORE_WARNING(message, ...)
#define B2D_CORE_ERROR(message, ...)

#define B2D_INFO(message, ...)
#define B2D_WARNING(message, ...)
#define B2D_ERROR(message, ...)

#define B2D_LOG_ASSERT(expr)

#endif