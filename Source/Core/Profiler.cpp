#include "B2D_pch.h"
#include "Profiler.h"

GenericStatisticDataList Profiler::msGenericStatistics;
GameSystemStatisticDataList Profiler::msGameSystemStatistics;
std::mutex Profiler::msMutex;
uint64 Profiler::frameId;
TimeStamp Profiler::msGameStart;
TimeStamp Profiler::msGameEnd;

void Profiler::OnGameSystemBeginFrame()
{
    frameId++;
    msGameSystemStatistics.clear();
    msGameStart = Clock::now();
}

void Profiler::OnGameSystemEndFrame()
{
    msGameEnd = Clock::now();
}

ScopedGenericStatistic::ScopedGenericStatistic(StatisticLabel label)
{
    mData.label = label;
}

ScopedGameSystemStatistic::ScopedGameSystemStatistic(GameSystemStatisticData::EType type, StatisticLabel label)
{
    mData.label = label;
    mData.type = type;
}
