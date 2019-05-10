#include "B2D_pch.h"
#include "Profiler.h"

ProfilerData Profiler::msScopedStatistics;
uint64 Profiler::msLastGameSystemFrameStartIndex = 0;
std::mutex Profiler::msMutex;

FORCEINLINE void Profiler::AddStatistic(StatisticData const& data)
{
    msMutex.lock();
    msScopedStatistics.emplace_back(data);
    msMutex.unlock();
}

ScopedStatistic::ScopedStatistic(StatisticTag const tag)
{
    mData.tag = tag;
    mData.thread = std::this_thread::get_id();
    mData.startTimestamp = Clock::now();
}

ScopedStatistic::~ScopedStatistic()
{
    mData.duration = Clock::now() - mData.startTimestamp;
    Profiler::AddStatistic(mData);
}

void Profiler::OnGameSystemNewFrame()
{
    msScopedStatistics.clear();
}

void Profiler::OnGameSystemEndFrame()
{

}
