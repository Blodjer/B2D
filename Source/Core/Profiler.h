#pragma once

using Clock = std::chrono::high_resolution_clock;
using TimeStamp = Clock::time_point; //std::chrono::time_point<Clock, std::chrono::duration<float, std::milli>>;
using Duration = Clock::duration; //std::chrono::duration<float, std::milli>;
using StatisticLabel = char const*;

struct StatisticDataBase
{
    std::thread::id thread;
    TimeStamp startTimestamp;
    Duration duration;
};

struct GenericStatisticData : StatisticDataBase
{
    StatisticLabel label;
};

struct GameSystemStatisticData : StatisticDataBase
{
    enum class EType
    {
        System,
        ThreadOverhead
    };

    StatisticLabel label;
    EType type;
};

template<class T>
class ScopedStatistic
{
public:
    ScopedStatistic()
    {
        B2D_STATIC_ASSERT_TYPE(StatisticDataBase, T);

        mData.thread = std::this_thread::get_id();
        mData.startTimestamp = Clock::now();
    }

    virtual ~ScopedStatistic()
    {
        mData.duration = Clock::now() - mData.startTimestamp;
        Profiler::AddStatistic<T>(mData);
    }

protected:
    T mData;
};

class ScopedGenericStatistic : public ScopedStatistic<GenericStatisticData>
{
public:
    ScopedGenericStatistic(StatisticLabel label);
};

class ScopedGameSystemStatistic : public ScopedStatistic<GameSystemStatisticData>
{
public:
    ScopedGameSystemStatistic(GameSystemStatisticData::EType type, StatisticLabel label);
};

#define PROFILE_SCOPE() _NODISCARD ScopedGenericStatistic _remitdepocs(__FUNCSIG__);

#define PROFILE_GAME_SYSTEM_BEGIN_FRAME() Profiler::OnGameSystemBeginFrame();
#define PROFILE_GAME_SYSTEM(label) ScopedGameSystemStatistic _remitdepocs(GameSystemStatisticData::EType::System, label);
#define PROFILE_GAME_SYSTEM_THREAD_OVERHEAD(label) ScopedGameSystemStatistic _remitdepocs(GameSystemStatisticData::EType::ThreadOverhead, label);
#define PROFILE_GAME_SYSTEM_END_FRAME() Profiler::OnGameSystemEndFrame();

using GenericStatisticDataList = std::vector<GenericStatisticData>;
using GameSystemStatisticDataList = std::vector<GameSystemStatisticData>;

class Profiler final
{
private:
    Profiler();

public:
    template<class T>
    FORCEINLINE static void AddStatistic(T const& data);

    template<>
    FORCEINLINE static void AddStatistic<GenericStatisticData>(GenericStatisticData const& data)
    {
        msMutex.lock();
        msGenericStatistics.emplace_back(data);
        msMutex.unlock();
    }

    template<>
    FORCEINLINE static void AddStatistic<GameSystemStatisticData>(GameSystemStatisticData const& data)
    {
        msMutex.lock();
        msGameSystemStatistics.emplace_back(data);
        msMutex.unlock();
    }

    static GenericStatisticDataList const& GetGenericStatistics() { return msGenericStatistics; }
    static uint64 GetGameSystemStatistics(GameSystemStatisticDataList const*& dataList, TimeStamp& start, TimeStamp& end) {
        start = msGameStart; end = msGameEnd; dataList = &msGameSystemStatistics; return frameId;
    }

    static void OnGameSystemBeginFrame();
    static void OnGameSystemEndFrame();

private:
    static GenericStatisticDataList msGenericStatistics;
    static GameSystemStatisticDataList msGameSystemStatistics;
    static std::mutex msMutex;
    static uint64 frameId;
    static TimeStamp msGameStart;
    static TimeStamp msGameEnd;
};
