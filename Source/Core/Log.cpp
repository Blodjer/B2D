#include "B2D_pch.h"
#include "Log.h"

#include "Debug/StackTraceHelper.h"

#include <spdlog/sinks/stdout_color_sinks.h>

void Log::Init(std::string const& applicationName)
{
    if (B2D_CHECKf(m_isInitialized, "Log is already initialized!"))
    {
        return;
    }

	spdlog::set_pattern("%^[%T] [%n] [%l]%$ %v");

	m_loggerCore = spdlog::stdout_color_mt("Core");

	m_loggerApp = spdlog::stdout_color_mt(applicationName);

    m_isInitialized = true;
}

void Log::Callstack(spdlog::level::level_enum level, STACKLOCATION_SIGNATURE, uint32 framesToSkip /*= 0*/)
{
    Log::Callstack(GetSink(STACKLOCATION_PARAM_FILE), level, STACKLOCATION_PARAMS, framesToSkip + 1);
}

void Log::Callstack(ESink sink, spdlog::level::level_enum level, STACKLOCATION_SIGNATURE, uint32 framesToSkip)
{
#ifdef B2D_BUILD_DEBUG
    std::string stackTrace;
    if (StackTraceHelper::GetStackTrace(stackTrace, framesToSkip + 1))
    {
        GetLogger(sink)->log(level, "Callstack:\n{}", stackTrace);
    }
    else
#endif
    {
        GetLogger(sink)->log(level, STACKLOCATION_LOG_FORMAT, STACKLOCATION_PARAMS);
    }
}
