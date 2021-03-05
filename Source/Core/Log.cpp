#include "B2D_pch.h"
#include "Log.h"

#include "Debug/StackTraceHelper.h"

#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Log::m_loggerCore;
std::shared_ptr<spdlog::logger> Log::m_loggerApp;

void Log::Init(std::string const& applicationName)
{
    if (B2D_CHECKf(m_isInitialized, "Log is already initialized!"))
    {
        return;
    }

	spdlog::set_pattern("%^[%T] [%n] [%l]%$ %v");

	m_loggerCore = spdlog::stdout_color_mt("Core");
	m_loggerCore->set_level(spdlog::level::debug);

	m_loggerApp = spdlog::stdout_color_mt(applicationName);
	m_loggerApp->set_level(spdlog::level::debug);

    m_isInitialized = true;
}

void Log::Callstack(spdlog::level::level_enum level, STACK_LOCATION_SIGNATURE, uint32 framesToSkip)
{
#ifdef B2D_BUILD_DEBUG
    std::string stackTrace;
    if (StackTraceHelper::GetStackTrace(stackTrace, framesToSkip + 1))
    {
        GET_LOGGER_FOR_FILE(file)->log(level, "Callstack:\n{}", stackTrace);
    }
    else
#endif
    {
        GET_LOGGER_FOR_FILE(file)->log(level, STACK_LOCATION_LOG_FORMAT, STACK_LOCATION_PARAMS);
    }
}
