#pragma once

#include "Core/Core.h"

#include <spdlog/spdlog.h>

class Log
{
private:
	static std::shared_ptr<spdlog::logger> mLoggerCore;
	static std::shared_ptr<spdlog::logger> mLoggerApp;

public:
	static std::shared_ptr<spdlog::logger> GetLoggerCore() { return mLoggerCore; }
	static std::shared_ptr<spdlog::logger> GetLoggerApp() { return mLoggerApp; }

	static void Init();

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
};

#define B2D_CORE_INFO(message, ...)		Log::Info(Log::GetLoggerCore(), message, __VA_ARGS__);
#define B2D_CORE_WARNING(message, ...)	Log::Warning(Log::GetLoggerCore(), message, __VA_ARGS__);
#define B2D_CORE_ERROR(message, ...)	Log::Error(Log::GetLoggerCore(), message, __VA_ARGS__);

#define B2D_INFO(message, ...)			Log::Info(Log::GetLoggerApp(), message, __VA_ARGS__);
#define B2D_WARNING(message, ...)		Log::Warning(Log::GetLoggerApp(), message, __VA_ARGS__);
#define B2D_ERROR(message, ...)			Log::Error(Log::GetLoggerApp(), message, __VA_ARGS__);