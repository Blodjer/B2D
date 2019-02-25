#include "B2D_pch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Log::mLoggerCore;
std::shared_ptr<spdlog::logger> Log::mLoggerApp;

void Log::Init()
{
	spdlog::set_pattern("%^[%T] [%n] [%l]%$ %v");

	mLoggerCore = spdlog::stdout_color_mt("Core");
	mLoggerCore->set_level(spdlog::level::debug);

	mLoggerApp = spdlog::stdout_color_mt("App");
	mLoggerApp->set_level(spdlog::level::debug);
}
