#include "B2D_pch.h"
#include "Profiler.h"

GenericStatisticDataList Profiler::ms_genericStatistics;
GameSystemStatisticDataList Profiler::ms_gameSystemStatistics;
std::mutex Profiler::ms_mutex;
uint64 Profiler::ms_frameId;
TimeStamp Profiler::ms_gameStart;
TimeStamp Profiler::ms_gameEnd;

void Profiler::OnGameSystemBeginFrame()
{
    ms_frameId++;
    ms_gameSystemStatistics.clear();
    ms_gameStart = Clock::now();
}

void Profiler::OnGameSystemEndFrame()
{
    ms_gameEnd = Clock::now();
}

ScopedGenericStatistic::ScopedGenericStatistic(StatisticLabel label)
{
    m_data.label = label;
}

ScopedGameSystemStatistic::ScopedGameSystemStatistic(GameSystemStatisticData::EType type, StatisticLabel label)
{
    m_data.label = label;
    m_data.type = type;
}
