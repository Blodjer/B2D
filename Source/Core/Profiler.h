#pragma once

using Clock = std::chrono::high_resolution_clock;
using TimeStamp = Clock::time_point; //std::chrono::time_point<Clock, std::chrono::duration<float, std::milli>>;
using Duration = Clock::duration; //std::chrono::duration<float, std::milli>;

using StatisticTag = char const*;

struct StatisticData
{
    StatisticTag tag;
    std::thread::id thread;
    TimeStamp startTimestamp;
    Duration duration;
};

class ScopedStatistic
{
public:
    ScopedStatistic(StatisticTag tag);
    ~ScopedStatistic();

public:
    StatisticData mData;
};

#define PROFILE_SCOPE() _NODISCARD ScopedStatistic _remitdepocs(__FUNCSIG__);
#define PROFILE_GAME_SYSTEM(name) _NODISCARD ScopedStatistic _remitdepocs(name);

#define PROFILE_GAME_SYSTEM_NEW_FRAME() Profiler::OnGameSystemNewFrame();
#define PROFILE_GAME_SYSTEM_END_FRAME() Profiler::OnGameSystemEndFrame();

using ProfilerData = std::vector<StatisticData>;

class Profiler final
{
private:
    Profiler();

public:
    template<class T>
    FORCEINLINE static void AddSystemStatistic(StatisticData const& data)
    {
        msStatistics[T::NAME] = data;
    }

    FORCEINLINE static void AddStatistic(StatisticData const& data);

    static ProfilerData const& GetStatistics() { return msScopedStatistics; }
    static ProfilerData const& GetLastFrame() { return msScopedStatistics; }

    static void OnGameSystemNewFrame();
    static void OnGameSystemEndFrame();

    static uint64 msLastGameSystemFrameStartIndex;

private:
    static ProfilerData msScopedStatistics;
    static std::mutex msMutex;
};
