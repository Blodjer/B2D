#include "B2D_pch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Log::m_loggerCore;
std::shared_ptr<spdlog::logger> Log::m_loggerApp;

void Log::Init(std::string const& applicationName)
{
	spdlog::set_pattern("%^[%T] [%n] [%l]%$ %v");

	m_loggerCore = spdlog::stdout_color_mt("Core");
	m_loggerCore->set_level(spdlog::level::debug);

	m_loggerApp = spdlog::stdout_color_mt(applicationName);
	m_loggerApp->set_level(spdlog::level::debug);
}
